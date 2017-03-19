#pragma once

#include <string>

// Forward declarations
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;

class BitmapFont {
	// Let TextLabel use BitmapFont's privates
	friend class TextLabel;
public:
	BitmapFont(std::string bmpFontFilename, int charWidth, int charHeight, char startChar, char endChar, SDL_Renderer* renderer);

	bool getCharacterSourceRect(char c, SDL_Rect* source /* out parameter */) const;

private:
	int mCharWidth;
	int mCharHeight;
	int mTextureWidth;
	char mStartChar;
	char mEndChar;
	SDL_Texture* mFontTexture;
};

