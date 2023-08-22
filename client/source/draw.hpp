#pragma once
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "squishy_array.hpp"

class SDL_Screen {
public:
	SDL_Screen();
	~SDL_Screen();
	void clearScreen(void);
	void finishDrawing(void);
	void drawText(char* inputText, int x, int y, float textSize);
	void drawImage(int imageId, int x, int y, int width, int height);
	void flipImage(int imageId);
	void drawRectangle(int x, int y, int width, int height, int color);
	void drawImageWithDir(int imageId, int x, int y, int width, int height, bool flip);
	int imageWidth(int imageId);
	int imageHeight(int imageId);
	void toggleWindowFullscreen(void);
	void showCursor(bool b_showCursor);
	bool getCursorState(void);
	bool isFullscreen(void);
	void drawImageWithRotation(int imageId, int x, int y, int width, int height, int rotation);
	void drawTextAligned(char* inputText, int x, int y, float textSize, int alignment);
private:
	//window rend vars
	SDL_Window *window = NULL;
	SDL_Surface *screen = NULL;
	SDL_Renderer *rend = NULL;
	//text vars
	TTF_Font* font;
	SDL_Surface* textSurf;
	SDL_Texture* textTexture;
	SDL_Rect textRect;
	const int text_load_size = 30;//px
	void loadTexture(char* filePath);
};

enum imageNums {
	IMAGE_MUSHROOM,
	IMAGE_PLAYER_TOP,
};

enum RECT_COLORS {
	CLR_BLU = 0,
	CLR_WHT,
	CLR_RED,
	CLR_GRN,
	CLR_BLK,
	CLR_YLW,
	CLR_GRY,
	CLR_LGY,
	CLR_RNG,
};

enum textalignments {
	ALIGN_RIGHT,
	ALIGN_CENTER,
	ALIGN_LEFT
};