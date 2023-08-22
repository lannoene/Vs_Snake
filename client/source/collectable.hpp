#pragma once

#include "squishy_array.hpp"
#include "draw.hpp"

class Collectable {
public:
	Collectable(int spawnX, int spawnY);
	~Collectable();
	int getX(void);
	int getY(void);
	void draw(SDL_Screen &Scene, int tileSize);
private:
	int x, y;
};