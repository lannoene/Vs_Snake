#include "hud.hpp"
#include "network.hpp"
#include "draw.hpp"

void drawHud(SquishyArray <Player*>&Player_Array, SDL_Screen &Scene) {
	char buffer[50];
	for (size_t i = 0; i < Player_Array.length(); i++) {
		if ((int)i != getClientId()) {
			snprintf(buffer, 50, "P%lld's score: %d", i + 1, Player_Array.data()[i]->getLength() - 2);
		} else {
			snprintf(buffer, 50, "Your score: %d", Player_Array.data()[i]->getLength() - 2);
		}
		Scene.drawTextAligned(buffer, 790, i*30, 30, ALIGN_RIGHT);
	}
}