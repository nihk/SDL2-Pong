#include "Paddle.h"
#include "Utils.h"

const float Paddle::Y_VELOCITY_DURING_UP_OR_DOWN_KEYPRESS = 150.0f;

Paddle::Paddle(SDL_Rect rect, SDL_Color color, int upKey, int downKey)
	: Entity(rect, color)
	, mUpKey(upKey)
	, mDownKey(downKey) {
}

bool Paddle::message(const MessageId messageId, void* data) {
	// IN: a uint8_t** which points to a pointer to the keys currently being pressed
	// OUT: n/a
	if (messageId == MessageId::SET_Y_VELOCITY) {
		float yVelocity = 0.0f;

		// Had to wrap the keys in a pointer since the method signature doesn't take const void* data,
		// which Game::mKeys is typed as
		if (uint8_t** pKeys = static_cast<uint8_t**>(data)) {
			const uint8_t* keys = *pKeys;

			if (keys[mUpKey]) {
				yVelocity = -Paddle::Y_VELOCITY_DURING_UP_OR_DOWN_KEYPRESS;
			} else if (keys[mDownKey]) {
				yVelocity = Paddle::Y_VELOCITY_DURING_UP_OR_DOWN_KEYPRESS;
			}

			setVelocityY(yVelocity);
		}

		// Always return true since the velocity should always be updated after receiving this message
		return true;
	}

	// IN: The Game's screen dimensions in the form of an int[] of size 2
	// OUT: n/a
	if (messageId == MessageId::Y_OUT_OF_SCREEN_BOUNDS) {
		if (int* screenDimens = static_cast<int*>(data)) {
			const int screenHeight = screenDimens[1];
			bool wasOutOfYBounds = false;

			if (mRect.y < 0) {
				mRect.y = 0;
				wasOutOfYBounds = true;
			}
			if (mRect.y + mRect.h > screenHeight) {
				mRect.y = screenHeight - mRect.h;
				wasOutOfYBounds = true;
			}

			// Return whether a correction was made to keep the Paddle within the screen height bounds
			return wasOutOfYBounds;
		}

		// Failed to cast data
		return false;
	}

	// IN: An entity that may have collided with the Paddle
	// OUT: an int* pointing to the angle the Entity should move towards after colliding with the paddle
	if (messageId == MessageId::HANDLE_COLLISION) {
		int segment;
		if (Entity* entity = static_cast<Entity*>(data)) {
			SDL_Point center = entity->getCenter();
			if (containsPoint(center)
					&& (segment = findSegmentContainingPoint(center)) != -1) {
				// Use data as an OUT parameter for the angle
				int* angle = static_cast<int*>(data);
				*angle = getAngleFromSegment(segment);

				return true;
			}
		}
		
		// No collision or failed to cast data
		return false;
	}

	return Entity::message(messageId, data);
}

int Paddle::getAngleFromSegment(int paddleSegment) {
	switch (paddleSegment) {
		case 0: return RandomHelpers::getRandomNumberWithinRange(-70, -35);
		case 1: return RandomHelpers::getRandomNumberWithinRange(-35, -5);
		case 2: return RandomHelpers::getRandomNumberWithinRange(-5, 5);
		case 3: return RandomHelpers::getRandomNumberWithinRange(5, 35);
		case 4: return RandomHelpers::getRandomNumberWithinRange(35, 70);
		default: return 0;
	}
}

int Paddle::findSegmentContainingPoint(SDL_Point point) const {
	// Round up for fractions of pixels so the whole paddle gets considered
	const int segmentHeight = static_cast<int>(ceil(mRect.h / static_cast<float>(NUM_SEGMENTS)));

	// Copy the full sized paddle then cut down its height to be segment sized
	SDL_Rect segment = mRect;
	segment.h = segmentHeight;

	for (size_t i = 0; i < NUM_SEGMENTS; ++i) {
		if (SDL_PointInRect(&point, &segment)) {
			return i;
		}
		segment.h += segmentHeight;
	}

	return -1;
}
