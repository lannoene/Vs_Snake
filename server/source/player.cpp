#include <random>

#include "player.hpp"
#include "squishy_array.hpp"
#include "network.hpp" // DO NOT INCLUDE IN HEADER. IT WILL CAUSE A RECURSIVE DEPENDENCY *_*

Player::Player() {
	this->x = 0;
	this->y = 0;
	this->length = 2;
	this->direction = RIGHT;
}

Player::~Player() {
	
}

int Player::getX(void) {
	return this->x;
}

int Player::getY(void) {
	return this->y;
}

int Player::getLength(void) {
	return this->length;
}

int Player::getDirection(void) {
	return this->direction;
}

void Player::setX(int newX) {
	this->x = newX;
}

void Player::setY(int newY) {
	this->y = newY;
}

void Player::setLength(int newLength) {
	this->length = newLength;
}

void Player::setDirection(int newDirection) {
	if (!b_hasChangedMovesThisTick) {
		this->direction = newDirection;
		this->b_hasChangedMovesThisTick = true;
	}
}

void Player::forceSetDirection(int newDirection) {
	this->direction = newDirection;
}

void Player::update(SquishyArray <Collectable*>&Collectable_Array, int tileSize, SquishyArray <Player*>&Player_Array) {
	if (this->b_isDead)
		return;
	this->b_hasChangedMovesThisTick = false;
	switch (this->direction) {
		case UP:
			if (this->y - tileSize >= 0)
				this->y -= tileSize;
		break;
		case DOWN:
			if (this->y + tileSize < WINDOW_HEIGHT)
				this->y += tileSize;
		break;
		case RIGHT:
			if (this->x + tileSize < WINDOW_WIDTH)
				this->x += tileSize;
		break;
		case LEFT:
			if (this->x - tileSize >= 0)
				this->x -= tileSize;
		break;
		case STOP:
		break;
	}
	
	for (size_t i = 0; i < Collectable_Array.length(); i++) { // this took me longer than i would care to admit to get it to stop segfaulting
		if (this->x == Collectable_Array.data()[i]->getX() && this->y == Collectable_Array.data()[i]->getY()) {
			Collectable_Array.array_splice(i, 1);
			if (i > 0)
				--i;
			for (int j = 0; j < (WINDOW_WIDTH * WINDOW_HEIGHT)/tileSize; j++) {
				int newXtry = (std::rand() % WINDOW_WIDTH/tileSize)*tileSize;
				int newYtry = (std::rand() % WINDOW_HEIGHT/tileSize)*tileSize;
				for (size_t k = 0; k < Collectable_Array.length(); k++) {
					if (newXtry != Collectable_Array.data()[k]->getX() && newYtry != Collectable_Array.data()[k]->getY()) { // trying to ensure no collectables get spawned on top of each other. also helps with randomizing...
						Collectable_Array.array_push(new Collectable(newXtry, newYtry));
						sendNewCollectableInfo(newXtry, newYtry);
						break;
					}
				}
				
				bool canSpawn = true;
				for (size_t k = 0; k < Player_Array.length(); k++) {
					int lastMoveX = Player_Array.data()[k]->getX();
					int lastMoveY = Player_Array.data()[k]->getY();
					for (size_t l = 0; l < Player_Array.data()[k]->LastMoves.length(); l++) {
						switch (Player_Array.data()[k]->LastMoves.data()[l]) {
							case LEFT:
								lastMoveX -= tileSize;
							break;
							case RIGHT:
								lastMoveX += tileSize;
							break;
							case UP:
								lastMoveY -= tileSize;
							break;
							case DOWN:
								lastMoveY += tileSize;
							break;
						}
						if (newXtry == lastMoveX && newYtry == lastMoveY) {
							canSpawn = false;
							break;
						}
					}
				}
				if (canSpawn) {
					Collectable_Array.array_push(new Collectable(newXtry, newYtry));
					sendNewCollectableInfo(newXtry, newYtry);
					break;
				}
			}
			this->length++;
		}
	}
	
	updateMoveArray();
}

void Player::updateMoveArray(void) {
	this->LastMoves.array_front(this->direction);
	
	LastMoves.array_splice(this->length, LastMoves.length() - this->length);
}

struct player_intersection_info Player::doTwoPlayersIntersect(SquishyArray <Player*>&Player_Array, int tileSize) {
	for (size_t i = 0; i < Player_Array.length(); i++) {
		if (Player_Array.data()[i]->getDeathStatus() == true)
			continue;
		
		if ((Player_Array.data()[i]->getX() - tileSize < 0 && Player_Array.data()[i]->getDirection() == LEFT) || (Player_Array.data()[i]->getY() - tileSize < 0 && Player_Array.data()[i]->getDirection() == UP) || (Player_Array.data()[i]->getX() + tileSize >= WINDOW_WIDTH && Player_Array.data()[i]->getDirection() == RIGHT) || (Player_Array.data()[i]->getY() + tileSize >= WINDOW_HEIGHT && Player_Array.data()[i]->getDirection() == DOWN))
			return player_intersection_info((int)i, -1, OUTOFBOUNDS);
		
		for (size_t j = 0; j < Player_Array.length(); j++) {
			if (Player_Array.data()[j]->getDeathStatus() == true)
				continue;
			if (Player_Array.data()[i]->getX() == Player_Array.data()[j]->getX() && Player_Array.data()[i]->getY() == Player_Array.data()[j]->getY() && i != j) {
				return player_intersection_info{(int)i, (int)j, HEADSHOT};
			}
			
			int lastMoveXp1 = Player_Array.data()[i]->getX();
			int lastMoveYp1 = Player_Array.data()[i]->getY();
			for (size_t k = 0; k < Player_Array.data()[j]->LastMoves.length(); k++) {
				switch (Player_Array.data()[j]->LastMoves.data()[k]) {
					case LEFT:
						lastMoveXp1 -= tileSize;
					break;
					case RIGHT:
						lastMoveXp1 += tileSize;
					break;
					case UP:
						lastMoveYp1 -= tileSize;
					break;
					case DOWN:
						lastMoveYp1 += tileSize;
					break;
				}
				
				if (Player_Array.data()[j]->getX() == lastMoveXp1 && Player_Array.data()[j]->getY() == lastMoveYp1 && i != j) {
					return player_intersection_info{(int)i, (int)j, SIDEHIT};
				} else if (Player_Array.data()[j]->getX() == lastMoveXp1 && Player_Array.data()[j]->getY() == lastMoveYp1 && i == j) {
					return player_intersection_info{(int)i, -1, SELFINFLICTED};
				}
			}
		}
	}
	return player_intersection_info{-1, -1, NONE};
}

bool Player::getDeathStatus(void) {
	return this->b_isDead;
}

void Player::setDeathStatus(bool status) {
	this->b_isDead = status;
}