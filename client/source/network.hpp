#pragma once

#include <iostream>

#include "player.hpp"
#include "squishy_array.hpp"
#include "collectable.hpp"

void connectToServer(char* serverAddr);
void checkForIncomingData(SquishyArray <Player*>&Player_Array, int &tileSize, SquishyArray <Collectable*>&Collectable_Array);
void sendRelevantData(void);
void sendNewControlPress(int direction);
void requestServerGameInfo(void);
int getKnownPlayerCount(void);
bool needsToResync(void);
int getClientId(void);
void forceResync(void);
class SquishyArray <int>* getDeadPlayers(void);
void requestRespawn(void);

struct player_update_pack {
	char magic[5];
	int playerX, playerY, playerLength, playerDirection, numOfCollectables;
	int playerId;
};

struct player_controlls_update {
	char magic[5];
	int direction;
};

struct player_small_update_pack {
	char magic[5];
	int moveId, x, y, numOfCollectables, numOfNewCollectables;
	int playerId;
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

enum server_statuses {
	WAITING_FOR_PLAYERS = 0,
	IN_GAME,
	ROUND_START,
	ROUND_FINISH,
};