#include "collectable.hpp"

Collectable::Collectable(int spawnX, int spawnY) {
	this->x = spawnX;
	this->y = spawnY;
}

Collectable::~Collectable(void) {
	
}

int Collectable::getX(void) {
	return this->x;
}

int Collectable::getY(void) {
	return this->y;
}

void Collectable::draw(SDL_Screen &Scene, int tileSize) {
	Scene.drawRectangle(this->x, this->y, tileSize, tileSize, CLR_RED);
}