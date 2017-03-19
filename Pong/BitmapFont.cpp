#include "BitmapFont.h"

#include <SDL.h>
#include <iostream>

BitmapFont::BitmapFont(std::string bmpFontFilename, int charWidth, int charHeight, char startChar, char endChar, SDL_Renderer* renderer)
	: mCharWidth(charWidth)
	, mCharHeight(charHeight)
	, mStartChar(startChar)
	, mEndChar(endChar) {

	// Gotta pass a const char pointer to this function. Alternatively use bmpFontFilename.c_str();
	SDL_Surface* surface = SDL_LoadBMP(bmpFontFilename.data());

	if (surface == nullptr) {
		// Did not load image
		std::cerr << "Could not load bitmap font: " << bmpFontFilename << SDL_GetError() << std::endl;
		mFontTexture = nullptr;  // alternatively, throw an exception
		return;
	}

	// SDL_Surfaces store dimensions of the image
	mTextureWidth = surface->w;

	// Mock the color that should be transparent
	SDL_SetColorKey(surface, 1 /* enable color key */, 0xFF00FF /* magenta */);

	// Create the texture (GFX card) from the surface (RAM)
	mFontTexture = SDL_CreateTextureFromSurface(renderer, surface);

	// Free the surface now that it's no longer necessary; the texture is all that's needed at this point
	SDL_FreeSurface(surface);
}

bool BitmapFont::getCharacterSourceRect(char c, SDL_Rect* source) const {
	// Out of bounds
	if (c < mStartChar || c > mEndChar) {
		return false;
	}

	// Since ASCII 0 is not necessarily the first character of the font image, establish an offset
	int offset = c - mStartChar;

	// This assumes there are no spaces between each individual letter in the font image
	// e.g. if the texture's width is 70px and each letter is 7px, there will be 10 letters per line
	int numCharsInLine = mTextureWidth / mCharWidth;

	// Populate the out parameter SDL_Rect, which will be the dimensional representation of the character c itself
	// Get the column of the character. This will be between 0 and numCharsInLine. Multiply by mCharWidth
	// to get the column of the character with respect to the font image
	source->x = (offset % numCharsInLine) * mCharWidth;
	// Get the row of the character. E.g. if the character is 30 and the numCharsInLine is 20, it will be on the second line (index 1)
	source->y = (offset / numCharsInLine) * mCharHeight;
	source->w = mCharWidth;
	source->h = mCharHeight;

	return true;
}
