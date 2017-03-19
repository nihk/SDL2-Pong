#pragma once

#include <SDL.h>

namespace Colors {
	static const SDL_Color MATTE_BLACK = { 0x2A, 0x2C, 0x2B, 0xFF };
	static const SDL_Color BLUE = { 0x29, 0x80, 0xB9, 0xFF };
	static const SDL_Color RED = { 0xE7, 0x4C, 0x3C, 0xFF };
	static const SDL_Color SOFT_GREEN = { 0x96, 0xED, 0x89, 0xFF };
	static const SDL_Color WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
}

namespace RandomHelpers {
	int getRandomNumberWithinRange(int min, int max);
	int getRandomNumberWithinTwoRanges(int min1, int max1, int min2, int max2);
	void seedRandomNumberGenerator(int seed);
}