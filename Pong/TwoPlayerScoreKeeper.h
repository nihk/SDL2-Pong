#pragma once

#include "BitmapFont.h"
#include "TextLabel.h"
#include "Messageable.h"
#include <vector>

struct SDL_Renderer;

class TwoPlayerScoreKeeper 
	: Messageable {
public:
	TwoPlayerScoreKeeper(SDL_Renderer* renderer, int centerScreenX, float fontSize, SDL_Color player1Color, SDL_Color player2Color);
	~TwoPlayerScoreKeeper();

	void draw(SDL_Renderer* renderer) const;
	bool message(const MessageId messageId, void* data = nullptr) override;

private:
	static const int PLAYER_1_KEY;
	static const int DIVIDER_KEY;
	static const int PLAYER_2_KEY;
	static const int Y_PADDING;

	int mPlayer1Score;
	int mPlayer2Score;
	BitmapFont* mBitmapFont;
	std::vector<TextLabel*> mTextLabels;
	int mCenterScreenX;
	float mFontSize;

	void refreshLabelPositions();
};

