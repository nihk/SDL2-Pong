#pragma once

#include "Entity.h"
#include <SDL.h>

class Paddle 
	: public Entity {
public:
	static const float Y_VELOCITY_DURING_UP_OR_DOWN_KEYPRESS;

	using Entity::Entity;
	Paddle(SDL_Rect rect, SDL_Color color, int upKey, int downKey);

	bool message(const MessageId messageId, void* data = nullptr) override;

private:
	static const uint8_t NUM_SEGMENTS = 5;

	int mUpKey;
	int mDownKey;

	static int getAngleFromSegment(int paddleSegment);

	int findSegmentContainingPoint(SDL_Point point) const;
};

