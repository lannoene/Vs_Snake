#include <iostream>

#include "network.hpp"
#include "game.hpp"
#include "draw.hpp"

int main(int argc, char* argv[]) {
	SDL_Screen Scene;
	char serverIp[20];
	printf("Enter server ip: ");
	scanf("%s", serverIp);
	connectToServer(serverIp);
	while (loadGame()) {
	}
	
	while (runGame(Scene)) {
	}
	
	return 0;
}