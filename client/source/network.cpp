#include <winsock2.h>
#include <ws2tcpip.h>

#include "network.hpp"

#define DEFAULT_PORT "2331"
#define DEFAULT_BUFLEN 16384

char sockBuffer[DEFAULT_BUFLEN];
bool b_needsToResync = true;
int thisClientServerId;
SquishyArray <int>deadPlayers(0);
int deadPlayerCount;

WSADATA wsaData;
//create wsapoll data
WSAPOLLFD fdarray = {};

struct server_info {
	SOCKET serverSock = INVALID_SOCKET;
	int playerCount;
} server;

void connectToServer(char* serverAddr) {
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		puts("could not create socket");
	}
	
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(serverAddr, DEFAULT_PORT, &hints, &result) != 0) {
		puts("getaddrinfo failed men");
		return;
	}
	
	for (ptr=result; ptr != NULL; ptr = ptr->ai_next) { // i literally have no idea why this needs to be a for loop :/
		server.serverSock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		
		if (server.serverSock == INVALID_SOCKET) {
			puts("socket could not be created");
			return;
		}
		
		if (connect(server.serverSock, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
			puts("could not connect");
			continue;
		}
		puts("did it");
		
		send(server.serverSock, (char*)"INFO", sizeof("INFO"), 0);
		
		fdarray.fd = server.serverSock;
		fdarray.events = POLLRDNORM;
		
		server.playerCount = -1;
		break;
		
	}
}

void checkForIncomingData(SquishyArray <Player*>&Player_Array, int &tileSize, SquishyArray <Collectable*>&Collectable_Array) {
	if (server.serverSock == INVALID_SOCKET)
		return;
	else if (WSAPoll(&fdarray, 1, 0) == SOCKET_ERROR) {
		puts("Timed out.");
		server.serverSock = INVALID_SOCKET;
		
	} else if (fdarray.revents & POLLRDNORM) {
		int size = recv(server.serverSock, (char*)&sockBuffer, DEFAULT_BUFLEN, 0);
		//printf("size: %d\n", size);
		if (size < 0) {
			puts("Read error. Recv return value less than 0.");
			return;
		}
		//FILE* fp = fopen("file.lvl", "wb");
		char magic[5];
		size_t readSockData = 0; // past tense read
		while (readSockData < (size_t)size) { // when it finishes reading a part of the buffer, the readSockData will increase so when the while loop restarts, the magic will be different & it will decode the next bit of data
			memcpy(magic, sockBuffer + readSockData, 5);
			if (memcmp(magic, "UPDP", 5) == 0) {
				player_update_pack gmUpdate = {};
				gmUpdate.playerLength = 0;
				
				for (size_t i = 0; i < Player_Array.length(); i++) {
					if (memcmp(sockBuffer + readSockData, "UPDP", 5) != 0) {
						char badMagic[5] = {};
						memcpy(badMagic, sockBuffer + readSockData, 5);
						printf("Could not read player %lld's update data. Bad magic result %s\n", i, badMagic);
						//fwrite(sockBuffer, 1, sizeof(sockBuffer), fp);
						break;
					}
					memcpy(&gmUpdate, sockBuffer + readSockData, sizeof(player_update_pack));
					Player_Array.data()[gmUpdate.playerId]->setX(gmUpdate.playerX);
					Player_Array.data()[gmUpdate.playerId]->setY(gmUpdate.playerY);
					Player_Array.data()[gmUpdate.playerId]->setLength(gmUpdate.playerLength);
					Player_Array.data()[gmUpdate.playerId]->setDirection(gmUpdate.playerDirection);
					char lastMoves[gmUpdate.playerLength];
					readSockData += sizeof(player_update_pack);
					memcpy(lastMoves, sockBuffer + readSockData, sizeof(char)*gmUpdate.playerLength); // i used to have &lastMoves. what was i thinking???? (better call saul reference XD)
					Player_Array.data()[gmUpdate.playerId]->LastMoves.array_splice(0, Player_Array.data()[gmUpdate.playerId]->LastMoves.length());
					for (int j = 0; j < gmUpdate.playerLength; j++) {
						Player_Array.data()[gmUpdate.playerId]->LastMoves.array_push(lastMoves[j]);
					}
					readSockData += sizeof(char)*gmUpdate.playerLength;
				}
				if (memcmp(sockBuffer + readSockData, "CUPD", 5) == 0) {
					Collectable_Array.array_splice(0, Collectable_Array.length());
					
					collectable_info_struct colUpdate = {};
					for (int i = 0; i < gmUpdate.numOfCollectables; i++) {
						if (memcmp(sockBuffer + readSockData, "CUPD", 5) == 0) {
							memcpy(&colUpdate, sockBuffer + readSockData, sizeof(collectable_info_struct));
							Collectable_Array.array_push(new Collectable(colUpdate.collectableX, colUpdate.collectableY));
							readSockData += sizeof(collectable_info_struct);
						}
					}
				}
				b_needsToResync = false;
			} else if (memcmp(magic, "INFO", 5) == 0) {
				server_info_struct sv_info;
				memcpy(&sv_info, sockBuffer + readSockData, sizeof(server_info_struct));
				server.playerCount = sv_info.playerCount;
				thisClientServerId = sv_info.playerCount - 1; // playerCount includes me, but arrays start with index 0, so joining making a playercount of 1 means my id would be 1 - 1 or 0
				tileSize = sv_info.tileSize;
				readSockData += sizeof(server_info_struct);
			} else if (memcmp(magic, "JOIN", 5) == 0) {
				Player_Array.array_push(new Player(true));
				printf("New id: %lld player joined!\n", Player_Array.shortLen());
				b_needsToResync = true;
				readSockData += sizeof("JOIN");
			} else if (memcmp(magic, "SMUP", 5) == 0) {
				player_small_update_pack smGmUpdate = {};
				if (memcmp(sockBuffer + readSockData, "SMUP", 5) == 0) {
					memcpy(&smGmUpdate, sockBuffer + readSockData, sizeof(player_small_update_pack));
					if (Player_Array.data()[smGmUpdate.playerId]->getX() != smGmUpdate.x || Player_Array.data()[smGmUpdate.playerId]->getY() != smGmUpdate.y) {
						Player_Array.data()[smGmUpdate.playerId]->update(Collectable_Array, tileSize);
					}
					Player_Array.data()[smGmUpdate.playerId]->setDirection(smGmUpdate.moveId);
					Player_Array.data()[smGmUpdate.playerId]->setX(smGmUpdate.x);
					Player_Array.data()[smGmUpdate.playerId]->setY(smGmUpdate.y);
					readSockData += sizeof(player_small_update_pack);
				}
			} else if (memcmp(magic, "PLDE", 5) == 0) {
				death_info_struct playerDeathInfo = {};
				memcpy(&playerDeathInfo, sockBuffer + readSockData, sizeof(playerDeathInfo));
				switch (playerDeathInfo.deathType) {
					case SIDEHIT:
						printf("Player %d crashed into player %d.\n", playerDeathInfo.player1id, playerDeathInfo.player2id);
					break;
					case HEADSHOT:
						printf("Player %d crashed into player %d's head. Ouch!\n", playerDeathInfo.player1id, playerDeathInfo.player2id);
					break;
					case SELFINFLICTED:
						printf("Player %d crashed into himself! Oof...\n", playerDeathInfo.player1id);
					break;
					case PASTDEATH:
						printf("Player %d died in the past.\n", playerDeathInfo.player1id);
					break;
					case OUTOFBOUNDS:
						printf("Player %d died by going out of bounds.\n", playerDeathInfo.player1id);
					break;
				}
				deadPlayerCount++;
				if (playerDeathInfo.player1id < (int)Player_Array.length()) {
					if (playerDeathInfo.deathType != HEADSHOT) {
						Player_Array.data()[playerDeathInfo.player1id]->setDeathStatus(true);
					} else if (playerDeathInfo.deathType == HEADSHOT) {
						Player_Array.data()[playerDeathInfo.player1id]->setDeathStatus(true);
						Player_Array.data()[playerDeathInfo.player2id]->setDeathStatus(true);
					}
				}
				readSockData += sizeof(playerDeathInfo);
				deadPlayers.array_push(playerDeathInfo.player1id);
			} else if (memcmp(magic, "NCOL", 5) == 0) {
				collectable_info_struct newColInfo;
				memcpy(&newColInfo, sockBuffer + readSockData, sizeof(collectable_info_struct));
				Collectable_Array.array_push(new Collectable(newColInfo.collectableX, newColInfo.collectableY));
				readSockData += sizeof(collectable_info_struct);
			} else if (memcmp(magic, "RSWN", 5) == 0) {
				respawn_info_struct respInfo;
				memcpy(&respInfo, sockBuffer + readSockData, sizeof(respawn_info_struct));
				Player_Array.data()[respInfo.playerId]->setX(0);
				Player_Array.data()[respInfo.playerId]->setY(0);
				Player_Array.data()[respInfo.playerId]->setDirection(RIGHT);
				Player_Array.data()[respInfo.playerId]->setLength(2);
				for (size_t j = 0; j < Player_Array.data()[respInfo.playerId]->LastMoves.length(); j++) {
					Player_Array.data()[respInfo.playerId]->LastMoves.data()[j] = RIGHT;
				}
				Player_Array.data()[respInfo.playerId]->setDeathStatus(false);
				readSockData += sizeof(respawn_info_struct);
			} else {
				//fwrite(sockBuffer, 1, sizeof(sockBuffer), fp);
				//printf("could not recognise magic: %s\n", magic);
				b_needsToResync = true;
				break;
			}
		}
	}
}

void sendRelevantData(void) {
	if (b_needsToResync) {
		send(server.serverSock, (char*)"SYRE", sizeof("SYRE"), 0);
	}
}

void sendNewControlPress(int direction) {
	// player_update_pack gmUpdate = {"UPDP", Player_Array.data()[PLAYER1]->getX(), Player_Array.data()[PLAYER1]->getY(), Player_Array.data()[PLAYER1]->getLength()};
	player_controlls_update controlUpdate = {"CNTR", direction};
	send(server.serverSock, (char*)&controlUpdate, sizeof(player_controlls_update), 0);
}

int getKnownPlayerCount(void) {
	return server.playerCount;
}

bool needsToResync(void) {
	return b_needsToResync;
}

int getClientId(void) {
	return thisClientServerId;
}

void forceResync(void) {
	b_needsToResync = true;
	send(server.serverSock, (char*)"SYRE", sizeof("SYRE"), 0);
}

class SquishyArray <int>* getDeadPlayers(void) {
	return &deadPlayers;
}

int getDeadPlayerCount(void) {
	return deadPlayerCount;
}

void requestRespawn(void) {
	send(server.serverSock, "RSWN", sizeof("RSWN"), 0);
}