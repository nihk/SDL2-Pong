#pragma once

#include "BitmapFont.h"
#include <SDL.h>

class TextLabel {
public:
	TextLabel(BitmapFont* bitmapFont);
	TextLabel(BitmapFont* bitmapFont, float fontSize, SDL_Color color);

	void setPosition(int x, int y);
	void setText(std::string text);
	void setFontSize(float size);
	void draw(SDL_Renderer* renderer);
	float getSingleCharacterWidth() const;

	float getTextWidth() const;
	float getTextHeight() const;

	SDL_Color getColor() const { return mColor; }
	void setColor(SDL_Color color) { mColor = color; }

private:
	BitmapFont* mBitmapFont;
	SDL_Color mColor;
	std::string mText;
	float mFontSize;  // px
	int mLetterSpacing;  // px
	int mXPos;
	int mYPos;
};

