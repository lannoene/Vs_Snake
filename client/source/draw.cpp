#include "draw.hpp"
SquishyArray <SDL_Texture*>sdl_image(0);

SDL_Screen::SDL_Screen() {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Vs. Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	TTF_Init();
	font = TTF_OpenFont("romfs/font/Arial.ttf", text_load_size);
	
	if (font == NULL) {
		puts("font is null");
	}
	loadTexture((char*)"romfs/mushroom.png");
}

SDL_Screen::~SDL_Screen() {
	for (size_t i = 0; i < sdl_image.length(); i++) {
		SDL_DestroyTexture(sdl_image.data()[i]);
	}
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(rend);
	SDL_Quit();
}

void SDL_Screen::clearScreen(void) {
	SDL_SetRenderDrawColor(rend, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend);
}

void SDL_Screen::drawImage(int imageId, int x, int y, int width, int height) {
	SDL_Rect rect = {x, y, width, height};
	SDL_RenderCopy(rend, sdl_image.data()[imageId], NULL, &rect);
}

void SDL_Screen::drawText(char* inputText, int x, int y, float textSize) {
	textSurf = TTF_RenderText_Solid(font, inputText, (SDL_Color){0,0,0, 255});

	float textMultiple = textSurf->h/textSize;
	
	int textWidth = textSurf->w/textMultiple;
	int textHeight = textSize;
	
	textRect = (SDL_Rect){x, y, textWidth, textHeight};
	textTexture = SDL_CreateTextureFromSurface(rend, textSurf);
	SDL_FreeSurface(textSurf);
	
	SDL_RenderCopy(rend, textTexture, NULL, &textRect);
	SDL_DestroyTexture(textTexture);
}

void SDL_Screen::finishDrawing(void) {
	SDL_RenderPresent(rend);
}

void SDL_Screen::loadTexture(char* filePath) {
	SDL_Surface* image_sur;
	image_sur = IMG_Load(filePath);
	if (image_sur == nullptr)
		printf("could not load texture: %s\n", filePath);
	SDL_Texture *t = SDL_CreateTextureFromSurface(rend, image_sur);
	sdl_image.array_push(t);
	SDL_FreeSurface(image_sur);
}

void SDL_Screen::drawRectangle(int x, int y, int width, int height, int color) {
	SDL_Rect rect = {x, y, width, height};
	switch (color) {
		case CLR_BLU:
			SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0xff, SDL_ALPHA_OPAQUE);
		break;
		case CLR_WHT:
			SDL_SetRenderDrawColor(rend, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
		break;
		case CLR_RED:
			SDL_SetRenderDrawColor(rend, 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE);
		break;
		case CLR_GRN:
			SDL_SetRenderDrawColor(rend, 0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE);
		break;
		case CLR_BLK:
			SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
		break;
		case CLR_YLW:
			SDL_SetRenderDrawColor(rend, 0xff, 0xff, 0x00, SDL_ALPHA_OPAQUE);
		break;
		case CLR_GRY:
			SDL_SetRenderDrawColor(rend, 0x80, 0x80, 0x80, SDL_ALPHA_OPAQUE);
		break;
		case CLR_LGY:
			SDL_SetRenderDrawColor(rend, 0xE0, 0xE0, 0xE0, SDL_ALPHA_OPAQUE);
		break;
		case CLR_RNG:
			SDL_SetRenderDrawColor(rend, 0xFF, 0x99, 0x00, SDL_ALPHA_OPAQUE);
		break;
	}
	SDL_RenderFillRect(rend, &rect);
}

void SDL_Screen::drawImageWithDir(int imageId, int x, int y, int width, int height, bool flip) {
	SDL_RendererFlip flipType;
	if (flip)
		flipType = SDL_FLIP_HORIZONTAL;
	else
		flipType = SDL_FLIP_NONE;
	
	SDL_Rect rect = {x, y, width, height};
	
	SDL_RenderCopyEx(rend, sdl_image.data()[imageId], NULL/*src rect.. could be useful for spritesheet*/, &rect, 0, NULL, flipType);

}

int SDL_Screen::imageWidth(int imageId) {
	int x;
	SDL_QueryTexture(sdl_image.data()[imageId], NULL, NULL, &x, NULL);
	return x;
}
int SDL_Screen::imageHeight(int imageId) {
	int y;
	SDL_QueryTexture(sdl_image.data()[imageId], NULL, NULL, NULL, &y);
	return y;
}

void SDL_Screen::toggleWindowFullscreen(void) {
	if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
		SDL_SetWindowFullscreen(window, 0);
		SDL_ShowCursor(true);
	} else {
		SDL_ShowCursor(false);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}
}

void SDL_Screen::showCursor(bool b_showCursor) {
	SDL_ShowCursor(b_showCursor);
}

bool SDL_Screen::getCursorState(void) {
	return SDL_ShowCursor(SDL_QUERY);
}

bool SDL_Screen::isFullscreen(void) {
	if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
		return true;
	} else {
		return false;
	}
}

void SDL_Screen::drawImageWithRotation(int imageId, int x, int y, int width, int height, int rotation) {
	SDL_Rect rect = {x, y, width, height};
	
	SDL_RenderCopyEx(rend, sdl_image.data()[imageId], NULL, &rect, rotation, NULL, SDL_FLIP_NONE);
}

void SDL_Screen::drawTextAligned(char* inputText, int x, int y, float textSize, int alignment) {
	textSurf = TTF_RenderText_Solid(font, inputText, (SDL_Color){0,0,0, 255});

	float textMultiple = textSurf->h/textSize;
	
	int textWidth = textSurf->w/textMultiple;
	int textHeight = textSize;
	
	switch (alignment) {
		case ALIGN_LEFT:
			textRect = (SDL_Rect){x, y, textWidth, textHeight};
		break;
		case ALIGN_RIGHT:
			textRect = (SDL_Rect){x - textWidth, y, textWidth, textHeight};
		break;
		case ALIGN_CENTER:
			textRect = (SDL_Rect){x - textWidth/2, y, textWidth, textHeight};
		break;
	}
	textTexture = SDL_CreateTextureFromSurface(rend, textSurf);
	SDL_FreeSurface(textSurf);
	
	SDL_RenderCopy(rend, textTexture, NULL, &textRect);
	SDL_DestroyTexture(textTexture);
}