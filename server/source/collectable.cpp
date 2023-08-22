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

void Collectable::eatCollectable(SquishyArray <Collectable*>&Collectables, int index) {
	
}