#include "TwoPlayerScoreKeeper.h"
#include "Utils.h"

const int TwoPlayerScoreKeeper::PLAYER_1_KEY = 0;
const int TwoPlayerScoreKeeper::DIVIDER_KEY = 1;
const int TwoPlayerScoreKeeper::PLAYER_2_KEY = 2;
const int TwoPlayerScoreKeeper::Y_PADDING = 30;

TwoPlayerScoreKeeper::TwoPlayerScoreKeeper(SDL_Renderer* renderer, int centerScreenX, float fontSize, SDL_Color player1Color, SDL_Color player2Color)
		: mPlayer1Score(0)
		, mPlayer2Score(0)
		, mBitmapFont(new BitmapFont("bmpfont_oldschool.bmp", 5, 7, ' ', '~', renderer))
		, mCenterScreenX(centerScreenX)
		, mFontSize(fontSize) {
	mTextLabels.emplace_back(new TextLabel(mBitmapFont, mFontSize, player1Color));
	mTextLabels.emplace_back(new TextLabel(mBitmapFont, mFontSize, Colors::WHITE));
	mTextLabels.emplace_back(new TextLabel(mBitmapFont, mFontSize, player2Color));
	refreshLabelPositions();
}

TwoPlayerScoreKeeper::~TwoPlayerScoreKeeper() {
	for (std::vector<TextLabel*>::size_type i = 0; i < mTextLabels.size(); ++i) {
		delete mTextLabels[i];
	}
	mTextLabels.clear();

	delete mBitmapFont;
	mBitmapFont = nullptr;
}

void TwoPlayerScoreKeeper::draw(SDL_Renderer* renderer) const {
	for (const auto& textLabel : mTextLabels) {
		textLabel->draw(renderer);
	}
}

bool TwoPlayerScoreKeeper::message(const MessageId messageId, void* data) {
	// IN: an int[] of size 2. index 0 is the value to increase the score of player 1, index
	// 1 is the value to increase the score of player 2.
	// OUT: n/a
	if (messageId == MessageId::UPDATE_SCORE) {
		if (int* scores = static_cast<int*>(data)) {
			mPlayer1Score += scores[0];
			mPlayer2Score += scores[1];
			refreshLabelPositions();
			return true;
		}

		// Failed to cast data
		return false;
	}

	return false;
}

void TwoPlayerScoreKeeper::refreshLabelPositions() {
	const std::string player1Score = std::to_string(mPlayer1Score) + " ";
	const std::string divider(":");
	const std::string player2Score = std::string(" ") + std::to_string(mPlayer2Score);

	// These calculations ensure the divider character is always positioned at the center of the screen.
	// Assumes all elements of mScoreLabels have the same single character width.
	const int characterWidth = static_cast<int>(mTextLabels.front()->getSingleCharacterWidth());

	const int player1ScoreXPosition = mCenterScreenX - (player1Score.size() * characterWidth + characterWidth / 2);
	const int dividerXPosition = mCenterScreenX - characterWidth / 2;
	const int player2ScoreXPosition = mCenterScreenX + characterWidth / 2;

	mTextLabels[PLAYER_1_KEY]->setText(player1Score);
	mTextLabels[PLAYER_1_KEY]->setPosition(player1ScoreXPosition, Y_PADDING);
	mTextLabels[DIVIDER_KEY]->setText(divider);
	mTextLabels[DIVIDER_KEY]->setPosition(dividerXPosition, Y_PADDING);
	mTextLabels[PLAYER_2_KEY]->setText(player2Score);
	mTextLabels[PLAYER_2_KEY]->setPosition(player2ScoreXPosition, Y_PADDING);
}
