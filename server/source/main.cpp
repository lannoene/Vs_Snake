#include <iostream>

#include "game.hpp"
#include "network.hpp"

int maxCollectables = 50;
int tileSize = 20;

int main(int argc, char* argv[]) {
	initSocket();
	
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-tilesize") == 0) {
				tileSize = atoi(argv[++i]);
				if (tileSize == 0) {
					puts("Tilesize cannot be 0");
					tileSize = 50;
				}
			} else if (strcmp(argv[i], "-maxcollectables") == 0) {
				maxCollectables = atoi(argv[++i]);
			}
		}
	} else {
		puts("Feel free to type in some command options as well such as -tilesize [your number] & -maxcollectables [your number]!");
	}
	
	while (loadGame(tileSize, maxCollectables)) {
		
	}
	
	
	while (runGame(tileSize, maxCollectables)) {
		
	}
	
	return 0;
}