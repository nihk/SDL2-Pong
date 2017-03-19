#include "Ball.h"
#include "Utils.h"

const float Ball::MIN_SPEED = 125.0f;  // px/sec
const float Ball::MAX_SPEED = 300.0f;  // px/sec
const float Ball::SPEED_INCREMENT = 25.0f;  // px/sec

Ball::Ball(SDL_Rect rect, SDL_Color color)
		: Entity(rect, color)
		, mCurrentSpeed(MIN_SPEED) {
	const float randomRads = getRandomValidBallRadians();
	setVelocity(static_cast<float>(cos(randomRads) * mCurrentSpeed), static_cast<float>(sin(randomRads) * mCurrentSpeed));
}

bool Ball::message(const MessageId messageId, void* data) {
	if (messageId == MessageId::PADDLE_BOUNCE) {
		if (int* degreesChange = static_cast<int*>(data)) {
			// Move back to the previously known position before a paddle collision
			setCenter(mPreviousCenter);

			// Paddle bounce dictates a speed increase
			if (mCurrentSpeed < MAX_SPEED) {
				mCurrentSpeed += SPEED_INCREMENT;
			}

			const bool isPositiveXVelocity = getVelocityX() > 0;
			// Since the degrees provided by Paddle::getAngleFromSegment only consider the left paddle, there 
			// needs to be some extra calculations to make it work for a bounce off of the right paddle, i.e. flip the 
			// direction then rotate by 180 degrees
			int newDegrees = *degreesChange * (isPositiveXVelocity ? -1 : 1) + (isPositiveXVelocity ? 180 : 0);
			float rads = static_cast<float>(newDegrees * M_PI / 180.0f);

			setVelocity(static_cast<float>(cos(rads) * mCurrentSpeed), static_cast<float>(sin(rads) * mCurrentSpeed));

			return true;
		}

		// Failed to cast data
		return false;
	}

	if (messageId == MessageId::Y_OUT_OF_SCREEN_BOUNDS) {
		if (int* screenDimens = static_cast<int*>(data)) {
			int screenHeight = screenDimens[1];
			SDL_Point center = getCenter();
			if (center.y < 0 || center.y > screenHeight) {
				// Revert to the position before going out of screen bounds
				setCenter(mPreviousCenter);
				// Flip the y-velocity for a top/bottom screen edge bounce
				setVelocityY(-getVelocityY());
				return true;
			}
		}

		// Did not go beyond top/bottom screen edges or failed to cast data
		return false;
	}

	if (messageId == MessageId::X_OUT_OF_SCREEN_BOUNDS) {
		if (int* screenDimens = static_cast<int*>(data)) {
			int screenWidth = screenDimens[0];
			int screenHeight = screenDimens[1];
			SDL_Point center = getCenter();

			// Now use data as an out param, to let the Game know which side the Ball went beyond
			int* xPos = static_cast<int*>(data);
			*xPos = center.x;

			// Reset the ball's position after it went beyond the screen's x bounds
			if (center.x < 0 || center.x > screenWidth) {
				setCenter(screenWidth / 2 - mRect.w, screenHeight / 2 - mRect.h);
				float randomRads = getRandomValidBallRadians();
				mCurrentSpeed = MIN_SPEED;
				setVelocity(static_cast<float>(cos(randomRads) * mCurrentSpeed), static_cast<float>(sin(randomRads) * mCurrentSpeed));

				return true;
			}
		}

		// Did not go beyond left/right screen edges or failed to cast data
		return false;
	}

	return Entity::message(messageId, data);
}

void Ball::update(float delta) {
	// Cache the previously known center position before an update in order to resolve bounces/out of screen bounds events
	mPreviousCenter = getCenter();
	Entity::update(delta);
}

// Gets a random direction for the Ball to head in. The range clamping guarantees it won't go nearly or directly upwards/downwards
float Ball::getRandomValidBallRadians() {
	return static_cast<float>(RandomHelpers::getRandomNumberWithinTwoRanges(120, 240, -60, 60) * M_PI / 180.0f);
}

