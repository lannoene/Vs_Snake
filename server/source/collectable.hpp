#pragma once

#include "squishy_array.hpp"

class Collectable {
public:
	Collectable(int spawnX, int spawnY);
	~Collectable();
	int getX(void);
	int getY(void);
	static void eatCollectable(SquishyArray <Collectable*>&Collectables, int index);
	int getNumOfNewCollectablesThisTick(void);
private:
	int x, y;
};