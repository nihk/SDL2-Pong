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

bool Ball::message(const MessageId messageId, const void* data, void* reply) {
	// data: an int* pointing to the new degrees the ball should direct its velocity towards
	// reply: nullptr
	if (messageId == MessageId::PADDLE_BOUNCE) {
		if (const int* degrees = static_cast<const int*>(data)) {
			// Move back to the previously known position before a paddle collision
			setCenter(mPreviousCenter);

			// Paddle bounce dictates a speed increase (capped)
			if (mCurrentSpeed < MAX_SPEED) {
				mCurrentSpeed += SPEED_INCREMENT;
			}

			const bool isPositiveXVelocity = getVelocityX() > 0;
			// Since the degrees provided by Paddle::getAngleFromSegment only consider the left paddle, there 
			// needs to be some extra calculations to make it work for a bounce off of the right paddle, i.e. flip the 
			// direction then rotate by 180 degrees
			const int adjustedDegrees = *degrees * (isPositiveXVelocity ? -1 : 1) + (isPositiveXVelocity ? 180 : 0);
			const float rads = static_cast<float>(adjustedDegrees * M_PI / 180.0f);

			setVelocity(static_cast<float>(cos(rads) * mCurrentSpeed), static_cast<float>(sin(rads) * mCurrentSpeed));

			return true;
		}

		// Failed to cast data
		return false;
	}

	// data: The Game's screen dimensions in the form of an int[] of size 2
	// reply: nullptr
	if (messageId == MessageId::Y_OUT_OF_SCREEN_BOUNDS) {
		if (const int* screenDimens = static_cast<const int*>(data)) {
			const int screenHeight = screenDimens[1];
			const SDL_Point center = getCenter();

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

	// data: The Game's screen dimensions in the form of an int[] of size 2
	// reply: an int[] of size 2 describing which side of the screen the ball breached: index 0 for the right side
	//		  of screen and index 1 for the left side of screen
	if (messageId == MessageId::X_OUT_OF_SCREEN_BOUNDS) {
		if (const int* screenDimens = static_cast<const int*>(data)) {
			const int screenWidth = screenDimens[0];
			const int screenHeight = screenDimens[1];
			const SDL_Point ballCenter = getCenter();

			const bool isBallRightOfScreen = ballCenter.x > screenWidth;
			const bool isBallLeftOfScreen = ballCenter.x < 0;

			if (isBallLeftOfScreen || isBallRightOfScreen) {
				// Now use reply as an out param, to let the Game know which side of the screen the Ball went beyond.
				int* ballScreenXBoundsStates = static_cast<int*>(reply);
				ballScreenXBoundsStates[0] = isBallRightOfScreen;
				ballScreenXBoundsStates[1] = isBallLeftOfScreen;

				// Reset the ball's position after it went beyond one of the screen's x bounds
				setCenter(screenWidth / 2 - mRect.w / 2, screenHeight / 2 - mRect.h / 2);
				const float randomRads = getRandomValidBallRadians();
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

