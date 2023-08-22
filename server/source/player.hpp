#pragma once

#include <iostream>
#include "squishy_array.hpp"
#include "collectable.hpp"

#define PLAYER1	0
#define PLAYER2	1
#define WINDOW_HEIGHT	600
#define WINDOW_WIDTH	800

struct player_intersection_info {
	int player1id, player2id, deathType;
};

class Player {
public:
	Player();
	~Player();
	int getX(void);
	int getY(void);
	int getLength(void);
	int getDirection(void);
	void setX(int newX);
	void setY(int newY);
	void setLength(int newLength);
	void setDirection(int direction);
	void update(SquishyArray <Collectable*>&Collectable_Array, int tileSize, SquishyArray <Player*>&Player_Array);
	void updateMoveArray(void);
	bool getDeathStatus(void);
	void setDeathStatus(bool status);
	static struct player_intersection_info doTwoPlayersIntersect(SquishyArray <Player*>&Player_Array, int tileSize);
	void forceSetDirection(int newDirection);
	
	SquishyArray <unsigned char>LastMoves{0};
private:
	int x = 0;
	int y = 0;
	int length = 0;
	int direction;
	bool b_hasChangedMovesThisTick = false;
	bool b_isDead = false;
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