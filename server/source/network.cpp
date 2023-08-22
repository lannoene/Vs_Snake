#include <iostream>

#include "network.hpp"

#define DEFAULT_PORT "2331"

struct addrinfo *result = NULL, *ptr = NULL, hints;
int connectedSockets;
void readyListenSocket(void);
SquishyArray <client_info_struct>clients{0};
SOCKET ListenSocket;
WSAEVENT NewEvent;
WSANETWORKEVENTS NetworkEvents;
//create wsapoll data
WSAPOLLFD fdarray = {};
char sockBuffer[DEFAULT_BUFLEN];

void initSocket(void) {
	readyListenSocket();
	
	//winsock event var
	NewEvent = WSACreateEvent();
	// with the listening socket and NewEvent listening for FD_ACCEPT
	WSAEventSelect(ListenSocket, NewEvent, FD_ACCEPT);
	fdarray.events = POLLRDNORM;
	
	struct hostent *thisHost;
	thisHost = gethostbyname("");
	printf("Tell your friends to connect to: %s (private ip)\n", inet_ntoa(*(struct in_addr *) *thisHost->h_addr_list));
}

void listenForIncomingClients(SquishyArray <Player*>&Player_Array) {
	if (ListenSocket == INVALID_SOCKET) {
		puts("LISTEN SOCKET INVALID/NOT READY");
		return;
	}
	
	if (WSAWaitForMultipleEvents(1, &NewEvent, FALSE, 0, FALSE) == WSA_WAIT_EVENT_0) {
		Player* playerPointer = new Player;
		clients.array_push(client_info_struct{INVALID_SOCKET, playerPointer, true});
		Player_Array.array_push(playerPointer);
		clients.data()[clients.shortLen()].ClientSock = accept(ListenSocket, NULL, NULL);
		
		if (clients.data()[clients.shortLen()].ClientSock == INVALID_SOCKET) {
			puts("Client could not be accepted (INVALID_SOCKET), rip.");
			closesocket(ListenSocket);
			clients.array_splice(clients.length() - 1, 1);
			readyListenSocket();
			return;
		}
		
		puts("New client joined!");
		
		for (size_t i = 0; i < clients.length() - 1; i++) {
			send(clients.data()[i].ClientSock, (char*)"JOIN", sizeof("JOIN"), 0);
		}
		
		if (WSAEnumNetworkEvents(ListenSocket, NewEvent, &NetworkEvents) > 0) {
			puts("error in wsaenumnetworkevents");
		}
		
		closesocket(ListenSocket);
		
		readyListenSocket();
		// with the listening socket and NewEvent listening for FD_ACCEPT
		WSAEventSelect(ListenSocket, NewEvent, FD_ACCEPT);
	}
}

void readyListenSocket(void) {
	WSADATA wsaData;
	ListenSocket = INVALID_SOCKET;
	
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		puts("wsastartup failed");
		return;
	}
	
	//create socket info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
	if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
		puts("getaddrinfo failed");
		
		return;
	}
	
	// Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	
	//we want to reuse the same address because we only have one address
	setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *) true, sizeof(true));
	
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	
	struct sockaddr_in sa;

	// store this IP address in sa:
	inet_pton(AF_INET, "0.0.0.0", &(sa.sin_addr));
	
	// Setup the TCP listening socket
	if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		puts("could not bind socket");
		return;
	}
	
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return; 
	}
	
	puts("Successfully readied listen socket");
}

void sendData(int socketNum, void* data, size_t size) { // probably gonna be unused ngl
	if (socketNum > 0) {
		send(clients.data()[socketNum].ClientSock, (char*)data, size, 0);
	} else {
		for (size_t i = 0; i < clients.length(); i++) {
			send(clients.data()[i].ClientSock, (char*)data, size, 0);
		}
	}
}

void sendCurrentGameInfo(SquishyArray <Player*>&Player_Array, SquishyArray <Collectable*>&Collectable_Array) {
	// resync package
	player_update_pack gmUpdate[clients.length()];
	// small package
	player_small_update_pack smGmUpdate[clients.length()];
	for (size_t i = 0; i < clients.length(); i++) {
		// loading sync pack
		gmUpdate[i] = {"UPDP", Player_Array.data()[i]->getX(), Player_Array.data()[i]->getY(), Player_Array.data()[i]->getLength(), Player_Array.data()[i]->getDirection(), (int)Collectable_Array.length(), (int)i};
		// loading small pack
		smGmUpdate[i] = {"SMUP", Player_Array.data()[i]->getDirection(), Player_Array.data()[i]->getX(), Player_Array.data()[i]->getY(), (int)Collectable_Array.length(), 0/*temp*/, (int)i};
	}
	
	for (size_t i = 0; i < clients.length(); i++) {
		char* sendBuff = (char*)malloc(DEFAULT_BUFLEN);
		int alreadyPutBytes = 0;
		if (clients.data()[i].b_needsToResync) {
			collectable_info_struct colUpdate[Collectable_Array.length()];
			for (size_t j = 0; j < Collectable_Array.length(); j++) {
				colUpdate[j] = {"CUPD", Collectable_Array.data()[j]->getX(), Collectable_Array.data()[j]->getY()};
			}
			for (size_t j = 0; j < clients.length(); j++) {
				memcpy(sendBuff + alreadyPutBytes, &gmUpdate[j], sizeof(player_update_pack));
				alreadyPutBytes += sizeof(player_update_pack);
				memcpy(sendBuff + alreadyPutBytes, Player_Array.data()[j]->LastMoves.data(), sizeof(char)*Player_Array.data()[j]->LastMoves.length());
				alreadyPutBytes += sizeof(char)*Player_Array.data()[j]->LastMoves.length();
			}
			memcpy(sendBuff + alreadyPutBytes, colUpdate, sizeof(collectable_info_struct)*Collectable_Array.length());
			alreadyPutBytes += sizeof(collectable_info_struct)*Collectable_Array.length();
			send(clients.data()[i].ClientSock, (char*)sendBuff, alreadyPutBytes, 0); // it's best to copy everything to a buffer and do only 1 send.
			clients.data()[i].b_needsToResync = false;
		} else {
			if (send(clients.data()[i].ClientSock, (char*)&smGmUpdate, sizeof(player_small_update_pack)*clients.length(), 0) == SOCKET_ERROR) {
				//printf("Socket %lld not connected.", i);
			}
		}
		free(sendBuff);
	}
}

void listenForIncomingData(SquishyArray <Player*>&Player_Array, int tileSize) {
	for (size_t i = 0; i < clients.length(); i++) {
		fdarray.fd = clients.data()[i].ClientSock;
		if (WSAPoll(&fdarray, 1, 0) == SOCKET_ERROR) {
			printf("Error while polling socket %lld. Disconnecting socket.\n", i);
			clients.array_splice(i, 1);
			return;
		}
		if (fdarray.revents & POLLRDNORM) {
			memset(sockBuffer, 0, sizeof(sockBuffer));
			int size = recv(clients.data()[i].ClientSock, (char*)&sockBuffer, DEFAULT_BUFLEN, 0);
			
			//printf("size: %d\n", size);
			
			if (size < 0) {
				puts("Read error. Recv return value less than 0.");
				return;
			}
			char magic[5];
			//printf("magic: %s\n", magic);
			size_t readSockData = 0; // past tense read
			while (readSockData < DEFAULT_BUFLEN) {
				memcpy(magic, sockBuffer + readSockData, 5);
				if (memcmp(magic, "UPDP", 5) == 0) {
					printf("recieved new client %lld update\n", i);
					player_update_pack gmUpdate;
					memcpy(&gmUpdate, sockBuffer + readSockData, sizeof(player_update_pack));
					readSockData += sizeof(player_update_pack);
					//printf("X: %d, Y: %d, Len: %d\n", gmUpdate.x, gmUpdate.y, gmUpdate.length);
				} else if (memcmp(magic, "CNTR", 5) == 0) {
					player_controlls_update ctrlUpdate;
					memcpy(&ctrlUpdate, sockBuffer + readSockData, sizeof(player_controlls_update));
					if (ctrlUpdate.direction == DOWN && Player_Array.data()[i]->getDirection() != UP)
						Player_Array.data()[i]->setDirection(ctrlUpdate.direction);
					else if (ctrlUpdate.direction == UP && Player_Array.data()[i]->getDirection() != DOWN)
						Player_Array.data()[i]->setDirection(ctrlUpdate.direction);
					else if (ctrlUpdate.direction == LEFT && Player_Array.data()[i]->getDirection() != RIGHT)
						Player_Array.data()[i]->setDirection(ctrlUpdate.direction);
					else if (ctrlUpdate.direction == RIGHT && Player_Array.data()[i]->getDirection() != LEFT)
						Player_Array.data()[i]->setDirection(ctrlUpdate.direction);
					readSockData += sizeof(player_controlls_update);
				} else if (memcmp(magic, "INFO", 5) == 0) {
					char buffer[DEFAULT_BUFLEN];
					int writtenBufSize = 0;
					server_info_struct sv_info = {"INFO", (int)clients.length(), tileSize};
					memcpy(buffer + writtenBufSize, &sv_info, sizeof(server_info_struct));
					writtenBufSize += sizeof(server_info_struct);
					readSockData += sizeof("INFO");
					for (size_t i = 0; i < clients.length(); i++) {
						if (Player_Array.data()[i]->getDeathStatus() == true) {
							death_info_struct deathInfo = {"PLDE", (int)i, (int)i, PASTDEATH};
							memcpy(buffer + writtenBufSize, &deathInfo, sizeof(death_info_struct));
							writtenBufSize += sizeof(death_info_struct);
						}
					}
					send(clients.data()[clients.shortLen()].ClientSock, buffer, writtenBufSize, 0);
				} else if (memcmp(magic, "SYRE", 5) == 0) {
					clients.data()[i].b_needsToResync = true;
					readSockData += sizeof("SYRE");
				} else if (memcmp(magic, "RSWN", 5) == 0) {
					if (Player_Array.data()[i]->getDeathStatus() == true) {
						Player_Array.data()[i]->setX(0);
						Player_Array.data()[i]->setY(0);
						Player_Array.data()[i]->forceSetDirection(RIGHT);
						Player_Array.data()[i]->setLength(2);
						for (size_t j = 0; j < Player_Array.data()[i]->LastMoves.length(); j++) {
							Player_Array.data()[i]->LastMoves.data()[j] = RIGHT;
						}
						Player_Array.data()[i]->setDeathStatus(false);
						respawn_info_struct respInfo = {"RSWN", (int)i};
						
						for (size_t i = 0; i < clients.length(); i++) {
							send(clients.data()[i].ClientSock, (char*)&respInfo, sizeof(respawn_info_struct), 0);
						}
					}
					readSockData += sizeof(respawn_info_struct);
				} else {
					break;
				}
			}
		}
	}
}

void sendPlayerDeathInfo(player_intersection_info intersectInfo) {
	death_info_struct playerDeathInfo = {"PLDE", intersectInfo.player1id, intersectInfo.player2id, intersectInfo.deathType};
	for (size_t i = 0; i < clients.length(); i++) {
		send(clients.data()[i].ClientSock, (char*)&playerDeathInfo, sizeof(death_info_struct), 0);
	}
}

void sendNewCollectableInfo(int x, int y) {
	collectable_info_struct newColUpdate = {"NCOL", x, y};
	for (size_t i = 0; i < clients.length(); i++) {
		send(clients.data()[i].ClientSock, (char*)&newColUpdate, sizeof(collectable_info_struct), 0);
	}
}