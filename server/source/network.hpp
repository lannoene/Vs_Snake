#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#include "squishy_array.hpp"
#include "player.hpp"

#define DEFAULT_BUFLEN 16384

struct client_info_struct {
	SOCKET ClientSock;
	Player* ClientPlayer;
	bool b_needsToResync;
};

void initSocket(void);
void listenForIncomingClients(SquishyArray <Player*>&Player_Array);
void sendData(int socketNum, void* data, size_t size);
void sendCurrentGameInfo(SquishyArray <Player*>&Player_Array, SquishyArray <Collectable*>&Collectable_Array);
void listenForIncomingData(SquishyArray <Player*>&Player_Array, int tileSize);
void sendPlayerDeathInfo(player_intersection_info intersectInfo);
void sendNewCollectableInfo(int x, int y);

struct player_update_pack {
	char magic[5];
	int playerX, playerY, playerLength, playerDirection, numOfCollectables;
	int playerId;
};

struct player_small_update_pack {
	char magic[5];
	int moveId, x, y, numOfCollectables, numOfNewCollectables;
	int playerId;
};

struct player_controlls_update {
	char magic[5];
	int direction;
};

struct server_info_struct {
	char magic[5];
	int playerCount, tileSize;
};

struct collectable_info_struct {
	char magic[5];
	int collectableX, collectableY;
};

struct death_info_struct {
	char magic[5];
	int player1id, player2id, deathType;
};

struct respawn_info_struct {
	char magic[5];
	int playerId;
};