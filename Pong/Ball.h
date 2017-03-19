#pragma once
#include "Entity.h"

class Ball 
	: public Entity {
public:
	static const float MIN_SPEED;
	static const float MAX_SPEED;
	static const float SPEED_INCREMENT;

	using Entity::Entity;
	Ball(SDL_Rect rect, SDL_Color color);

	bool message(const MessageId messageId, void* data = nullptr) override;
	void update(float delta) override;

private:
	float mCurrentSpeed;
	SDL_Point mPreviousCenter;

	static float getRandomValidBallRadians();
};

