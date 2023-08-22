#include <random>
#include <ctime>

#include "game.hpp"
#include "network.hpp"
#include "frame_cap.hpp"
#include "collectable.hpp"

SquishyArray <Player*>Player_Array(0);
SquishyArray <Collectable*>Collectable_Array(0);

int gameFrame = 0;

bool runGame(int tileSize, int maxCollectables) {
	
	listenForIncomingClients(Player_Array);
	listenForIncomingData(Player_Array, tileSize);
	
	player_intersection_info intersectInf = Player::doTwoPlayersIntersect(Player_Array, tileSize);
	
	if (intersectInf.deathType != NONE) {
		printf("Player %d crashed into player %d. Type of death: %d.\n", intersectInf.player1id, intersectInf.player2id, intersectInf.deathType);
		sendPlayerDeathInfo(intersectInf);
		if (intersectInf.deathType != HEADSHOT) {
			Player_Array.data()[intersectInf.player1id]->setDeathStatus(true);
		} else if (intersectInf.deathType == HEADSHOT) {
			Player_Array.data()[intersectInf.player1id]->setDeathStatus(true);
			Player_Array.data()[intersectInf.player2id]->setDeathStatus(true);
		}
	}
	
	if (gameFrame % 4 == 0 && Player_Array.length() > 0) {
		for (size_t i = 0; i < Player_Array.length(); i++) {
			Player_Array.data()[i]->update(Collectable_Array, tileSize, Player_Array);
		}
		sendCurrentGameInfo(Player_Array, Collectable_Array);
	}
	
	frameCap_DelayNextFrame();
	
	gameFrame++;
	
	return true;
}

bool loadGame(int tileSize, int maxCollectables) {
	std::srand(std::time(nullptr));
	
	for (int i = 0; i < std::rand() % std::rand(); i++) {
		std::rand(); // just doing some funky stuff to try to make it more unpredictable
	}
	
	for (int i = 0; i < maxCollectables; i++) {
		Collectable_Array.array_push(new Collectable((int)(std::rand() % WINDOW_WIDTH/tileSize)*tileSize, (int)(std::rand() % WINDOW_HEIGHT/tileSize)*tileSize));
	}
	return false;
}