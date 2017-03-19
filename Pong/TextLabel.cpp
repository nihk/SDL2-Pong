#include "TextLabel.h"
#include <SDL.h>
#include "Utils.h"

TextLabel::TextLabel(BitmapFont* bitmapFont)
	: TextLabel(bitmapFont, 18.0f, Colors::WHITE) {
}

TextLabel::TextLabel(BitmapFont* bitmapFont, float fontSize, SDL_Color color)
	: mBitmapFont(bitmapFont)
	, mColor(color)
	, mText("")
	, mFontSize(fontSize)
	, mLetterSpacing(1)
	, mXPos(0)
	, mYPos(0) {
}

void TextLabel::setPosition(int x, int y) {
	mXPos = x;
	mYPos = y;
}

void TextLabel::setText(std::string text) {
	mText = text;
}

void TextLabel::setFontSize(float size) {
	mFontSize = size;
}

void TextLabel::draw(SDL_Renderer* renderer) {
	// I had the TextLabels share the same BitmapFont, so they'll have to set their individual
	// text colors each time they are drawn
	SDL_SetTextureColorMod(mBitmapFont->mFontTexture, mColor.r, mColor.g, mColor.b);

	// Font size is effectively the height that the font should be. I need to scale the width to that.
	// E.g. if the font size (i.e. the desired height of each character) is 20 and the original character 
	// height is 10, the desired width will be twice the width of the original character.
	int destinationWidth = static_cast<int>(mFontSize / mBitmapFont->mCharHeight * mBitmapFont->mCharWidth);
	// This will be the output dimensions for the character
	SDL_Rect destinationRect;
	destinationRect.w = destinationWidth;
	destinationRect.h = static_cast<int>(mFontSize);
	// The yposition will never change so long as I only want to print 1 line of text
	destinationRect.y = mYPos;

	for (std::string::size_type i = 0; i < mText.size(); ++i) {
		char c = mText[i];
		// The input character dimensions based on the font image
		SDL_Rect sourceRect;
		// mXPos is always the x position where the first character will start. This loop
		// adds to that to get the next character's position.
		// Because each successive character creates more and more letter spaces, must compensate for that, too.
		// E.g. H e l l o starting at x=100 with each character being 35px wide and a spacing of 1px would be:
		// H = 100+0+0, e = 100+35+1, l = 100+70+2, l = 100+105+3, etc.
		destinationRect.x = mXPos + (destinationRect.w * i) + (mLetterSpacing * i);
		if (mBitmapFont->getCharacterSourceRect(c, &sourceRect)) {
			// Take the position of the sourceRect on the texture and map that onto the renderer; that
			// latter location is denoted by the destinationRect position.
			// Note: the destinationRect changes based on the font size. It will stretch/shrink the image
			// captured by the sourceRect accordingly.
			SDL_RenderCopy(renderer, mBitmapFont->mFontTexture, &sourceRect, &destinationRect);
		}
	}
}

float TextLabel::getSingleCharacterWidth() const {
	return mFontSize / mBitmapFont->mCharHeight * mBitmapFont->mCharWidth + mLetterSpacing;
}

float TextLabel::getTextWidth() const {
	return getSingleCharacterWidth() * mText.size();
}

float TextLabel::getTextHeight() const {
	return mFontSize;
}
