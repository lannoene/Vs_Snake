#include "player.hpp"
#include "squishy_array.hpp"
#include "collectable.hpp"

Player::Player(bool serverRunPlayer) {
	if (serverRunPlayer) {
		
	}
	this->x = 0;
	this->y = 0;
	this->length = 0;
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

void Player::setX(int newX) {
	this->x = newX;
}

void Player::setY(int newY) {
	this->y = newY;
}

void Player::setLength(int newLength) {
	this->length = newLength;
}

int Player::getDirection(void) {
	return this->direction;
}

void Player::setDirection(int newDirection) {
	this->direction = newDirection;
}

void Player::pleaseSetDirection(int newDirection) { // how polite!
	if (!b_hasChangedMovesThisTick) {
		this->direction = newDirection;
		this->b_hasChangedMovesThisTick = true;
	}
}

void Player::draw(SDL_Screen &Scene, int tileSize, int playerId) {
	if (this->b_isDead) {
		return;
	}
	int lastDrawnX = this->x;
	int lastDrawnY = this->y;
	for (size_t i = 0; i < LastMoves.length(); i++) {
		if (!i) {
			switch ((playerId % 4)) {
				case 0:
					Scene.drawRectangle(lastDrawnX, lastDrawnY, tileSize, tileSize, CLR_BLU);
				break;
				case 1:
					Scene.drawRectangle(lastDrawnX, lastDrawnY, tileSize, tileSize, CLR_GRN);
				break;
				case 2:
					Scene.drawRectangle(lastDrawnX, lastDrawnY, tileSize, tileSize, CLR_GRY);
				break;
				case 3:
					Scene.drawRectangle(lastDrawnX, lastDrawnY, tileSize, tileSize, CLR_RNG);
				break;
			}
		} else if (i) {
			Scene.drawRectangle(lastDrawnX, lastDrawnY, tileSize, tileSize, CLR_YLW);
		}
		switch (LastMoves.data()[i]) {
			case LEFT:
				lastDrawnX += tileSize;
			break;
			case RIGHT:
				lastDrawnX -= tileSize;
			break;
			case UP:
				lastDrawnY += tileSize;
			break;
			case DOWN:
				lastDrawnY -= tileSize;
			break;
			default:
				//printf("%d\n", LastMoves.data()[i]);
			break;
		}
	}
}

void Player::update(SquishyArray <Collectable*>&Collectable_Array, int tileSize) {
	/*switch (this->direction) {
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
	}*/
	for (size_t i = 0; i < Collectable_Array.length(); i++) {
		if (this->x == Collectable_Array.data()[i]->getX() && this->y == Collectable_Array.data()[i]->getY()) {
			Collectable_Array.array_splice(i, 1);
			if (i > 0)
				--i;
			this->length++; // client side, will get synced with the server
		}
	}
	
	updateMoveArray();
	this->b_hasChangedMovesThisTick = false;
}

void Player::updateMoveArray(void) {
	this->LastMoves.array_front(this->direction);
	
	LastMoves.array_splice(this->length, LastMoves.length() - this->length);
}

bool Player::getDeathStatus(void) {
	return this->b_isDead;
}

void Player::setDeathStatus(bool status) {
	this->b_isDead = status;
}