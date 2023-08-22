#pragma once

#include <iostream>
#include "draw.hpp"
#include "squishy_array.hpp"
#include "collectable.hpp"

#define PLAYER1 0

class Player {
public:
	Player(bool serverRunPlayer);
	~Player();
	int getX(void);
	int getY(void);
	int getLength(void);
	int getDirection(void);
	void setX(int newX);
	void setY(int newY);
	void setLength(int newLength);
	void setDirection(int newDirection);
	void draw(SDL_Screen &Scene, int tileSize, int playerId);
	void update(SquishyArray <Collectable*>&Collectable_Array, int tileSize);
	void updateMoveArray(void);
	void pleaseSetDirection(int newDirection);
	bool getDeathStatus(void);
	void setDeathStatus(bool status);
	
	SquishyArray <int>LastMoves{0};
private:
	int x = 0;
	int y = 0;
	int length = 0;
	int direction;
	bool b_isDead = false;
	bool b_hasChangedMovesThisTick = false;
};

enum directions {
	LEFT = 0,
	RIGHT,
	UP,
	DOWN,
	STOP,
};

enum typeOfDeath {
	NONE = 0,
	SIDEHIT,
	HEADSHOT,
	SELFINFLICTED,
	PASTDEATH,
	OUTOFBOUNDS
};