#include "game.hpp"
#include "player.hpp"
#include "squishy_array.hpp"
#include "draw.hpp"
#include "frame_cap.hpp"
#include "hud.hpp"

SquishyArray <Player*>Player_Array(0);
SquishyArray <Collectable*>Collectable_Array(0);

int tileSize = 50;
int gameFrame = 0;
int maxCollectables = 10;

bool runGame(SDL_Screen &Scene) {
	checkForIncomingData(Player_Array, tileSize, Collectable_Array);
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				return false;
			break;
			case SDL_KEYDOWN:
				if (event.key.repeat)
					break;
				switch (event.key.keysym.scancode) {
					default:
					break;
					case SDL_SCANCODE_S:
						if (Player_Array.data()[getClientId()]->getDirection() != UP && Player_Array.data()[getClientId()]->getDirection() != DOWN) {
							sendNewControlPress(DOWN);
							Player_Array.data()[getClientId()]->pleaseSetDirection(DOWN);
						}
					break;
					case SDL_SCANCODE_W:
						if (Player_Array.data()[getClientId()]->getDirection() != DOWN && Player_Array.data()[getClientId()]->getDirection() != UP) {
							sendNewControlPress(UP);
							Player_Array.data()[getClientId()]->pleaseSetDirection(UP);
						}
					break;
					case SDL_SCANCODE_A:
						if (Player_Array.data()[getClientId()]->getDirection() != RIGHT && Player_Array.data()[getClientId()]->getDirection() != LEFT) {
							sendNewControlPress(LEFT);
							Player_Array.data()[getClientId()]->pleaseSetDirection(LEFT);
						}
					break;
					case SDL_SCANCODE_D:
						if (Player_Array.data()[getClientId()]->getDirection() != LEFT && Player_Array.data()[getClientId()]->getDirection() != RIGHT) {
							sendNewControlPress(RIGHT);
							Player_Array.data()[getClientId()]->pleaseSetDirection(RIGHT);
						}
					break;
					case SDL_SCANCODE_R:
						requestRespawn();
					break;
					/*case SDL_SCANCODE_RETURN:
						sendNewControlPress(STOP);
						Player_Array.data()[getClientId()]->pleaseSetDirection(STOP);
					break;*/
				}
			break;
		}
	}
	
	Scene.clearScreen();
	
	sendRelevantData();
	if (gameFrame % 20 == 0) {
		forceResync();
	}
	
	if (gameFrame % 4 == 0) {
		for (size_t i = 0; i < Player_Array.length(); i++) {
			if (!needsToResync()) {
				//Player_Array.data()[i]->update(tileSize); this is now done inside network.cpp in order to sync better with the server.
			}
		}
	}
	
	for (int i = 0; i < WINDOW_WIDTH/tileSize; i++) {
		for (int j = 0; j < WINDOW_HEIGHT/tileSize; j++) {
			if ((i % 2 == 0 && j % 2 == 0) || (j % 2 != 0 && i % 2 != 0))
				Scene.drawRectangle(i*tileSize, j*tileSize, tileSize, tileSize, CLR_LGY);
		}
	}
	
	for (size_t i = 0; i < Collectable_Array.length(); i++) {
		Collectable_Array.data()[i]->draw(Scene, tileSize);
	}
	for (size_t i = 0; i < Player_Array.length(); i++) {
		Player_Array.data()[i]->draw(Scene, tileSize, (int)i);
	}
	
	drawHud(Player_Array, Scene);
	
	Scene.finishDrawing();
	
	gameFrame++;
	frameCap_DelayNextFrame();
	
	return true;
}

bool loadGame(void) {
	checkForIncomingData(Player_Array, tileSize, Collectable_Array);
	
	if (getKnownPlayerCount() != -1) {
		for (int i = 0; i < getKnownPlayerCount(); i++) { // includes you
			Player_Array.array_push(new Player(true));
			SquishyArray <int>*deadPlayerIds = getDeadPlayers();
			for (size_t j = 0; j < deadPlayerIds->length(); j++)
				if (deadPlayerIds->data()[j] == i)
					Player_Array.data()[i]->setDeathStatus(true);
		}
		printf("%d players in server, %lld players allocated.\n", getKnownPlayerCount(), Player_Array.length());
		return false;
	}
	
	return true;
}