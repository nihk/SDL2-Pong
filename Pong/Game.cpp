#include "Game.h"
#include "Utils.h"
#include "Paddle.h"
#include "TwoPlayerScoreKeeper.h"
#include <iostream>
#include <SDL.h>
#include <ctime>

Game::Game()
	: mScreenWidth(600)
	, mScreenHeight(300)
	, mShouldQuit(false)
	, mWindow(nullptr)
	, mRenderer(nullptr)
	, mBackgroundColor(Colors::MATTE_BLACK)
	, mKeys(nullptr)
	, mBall(nullptr)
	, mScoreKeeper(nullptr) {
}

bool Game::run() {
	if (!initialize()) {
		std::cerr << "*** Game initialization failed" << std::endl;
		return false;
	}

	const float FPS = 30.0f;
	const float DELTA_F = 1.0f / FPS;
	const unsigned DELTA_MS = static_cast<int>(DELTA_F * 1000.0f);
	unsigned currentTime = SDL_GetTicks();
	unsigned gameTime = currentTime;
	const int FRAME_SKIP = 6;

	while (!mShouldQuit) {
		processEvents();

		int updates = 0;
		while ((currentTime = SDL_GetTicks()) > gameTime && ++updates < FRAME_SKIP) {
			gameTime += DELTA_MS;
			update(DELTA_F);
		}

		draw();
	}

	shutdown();

	return true;
}

bool Game::initialize() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "*** Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	mWindow = SDL_CreateWindow("Nick Rose: Assignment 2",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		mScreenWidth, mScreenHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!mWindow) {
		std::cerr << "*** Failed to create window: " << SDL_GetError() << std::endl;
		return false;
	}

	// Get a pointer to keyboard state managed by SDL
	mKeys = SDL_GetKeyboardState(nullptr);

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!mRenderer) {
		std::cerr << "*** Failed to create renderer: " << SDL_GetError() << std::endl;
		return false;
	}

	RandomHelpers::seedRandomNumberGenerator(static_cast<int>(time(nullptr)));

	load();

	return true;
}

void Game::load() {
	// I could've used the stack for all these class members, but I wanted to try using the heap, instead
	int padding = 40;
	int paddleWidth = 15;
	int paddleHeight = 60;
	SDL_Rect firstPaddleDimens = { padding, padding, paddleWidth, paddleHeight };
	SDL_Color firstPaddleColor = Colors::RED;
	SDL_Rect secondPaddleDimens = { mScreenWidth - padding - paddleWidth, mScreenHeight - padding - paddleHeight, paddleWidth, paddleHeight };
	SDL_Color secondPaddleColor = Colors::BLUE;
	mPaddles.emplace_back(new Paddle(firstPaddleDimens, firstPaddleColor, SDL_SCANCODE_W, SDL_SCANCODE_S));
	mPaddles.emplace_back(new Paddle(secondPaddleDimens, secondPaddleColor, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN));

	int ballSideLength = 10;
	SDL_Rect ballDimens = { mScreenWidth / 2 - ballSideLength / 2, mScreenHeight / 2 - ballSideLength / 2, ballSideLength, ballSideLength };
	mBall = new Ball(ballDimens, Colors::SOFT_GREEN);

	mScoreKeeper = new TwoPlayerScoreKeeper(mRenderer, mScreenWidth / 2, 30.0f /* fontSize */, firstPaddleColor, secondPaddleColor);
}

void Game::unload() {
	delete mScoreKeeper;
	mScoreKeeper = nullptr;

	for (std::vector<Paddle*>::size_type i = 0; i < mPaddles.size(); ++i) {
		delete mPaddles[i];
	}
	mPaddles.clear();

	delete mBall;
	mBall = nullptr;
}

void Game::shutdown() {
	std::cout << "Shutting down game" << std::endl;

	unload();

	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();
}

void Game::update(float delta) {
	// I wanted to treat the Paddles and Ball as encapsulated as possible by only interacting
	// with them via messaging. It ended up being a bit confusing using the void* data parameter
	// with both IN and OUT contexts. In future I think I'd probably have the message signature
	// be something like message(MessageId, void* data, void* reply) to clarify things a bit more.

	// Pass the screen dimension data with messages to keep the Entities within screen bounds
	int screenDimens[] = { mScreenWidth, mScreenHeight };
	void* data = screenDimens;

	// First handle where the paddle should update to
	for (const auto& paddle: mPaddles) {
		// Set the paddles' velocities based on the key states
		if (paddle->message(MessageId::SET_Y_VELOCITY, &mKeys)) {
			paddle->update(delta);
			// Handle paddles going off the top/bottom of screen
			paddle->message(MessageId::Y_OUT_OF_SCREEN_BOUNDS, data);
		}
	}

	//Next, try to perform Ball screen bounds handling
	mBall->update(delta);
	// This flag being true means the Ball received a message that changed
	// its state in such a way that it should be updated once again
	bool shouldReupdateBall = false;

	// Check if ball went beyond top/bottom of screen
	if (mBall->message(MessageId::Y_OUT_OF_SCREEN_BOUNDS, data)) {
		shouldReupdateBall = true;

	} else if (mBall->message(MessageId::X_OUT_OF_SCREEN_BOUNDS, data)) {
		// data was used as an out parameter here, it now contains the details of which
		// screen sides the ball has breached. This conveniently translates into a value to increase the 
		// score by. That is, if the ball went beyond the left side of the screen that value would be 1 (i.e. "true"). 
		// The score of player 2 is thus increased by that same value.
		mScoreKeeper->message(MessageId::UPDATE_SCORE, data);
		shouldReupdateBall = true;

	} else {  
		// Otherwise check for Ball collision with paddles.
		// Reassign data with a pointer to a copy of the ball. I copied it because I wanted to use
		// data as an out parameter when messaging the paddles. If I sent instead the pointer to mBall,
		// it would've been corrupted by the out parameter manipulation. The overhead of this copy
		// could be relieved by having the Messageable::message() signature include a void* reply parameter
		// and using that for the "out" part instead of the single data parameter as both IN and OUT; I could
		// then just pass a const pointer to mBall and not a copy.
		Entity entity = *mBall;
		data = &entity;

		for (const auto& paddle : mPaddles) {
			if (paddle->message(MessageId::HANDLE_COLLISION, data)) {
				// Update it's new velocity based on that collision, using the newly assigned data pointer out value
				mBall->message(MessageId::PADDLE_BOUNCE, data);
				shouldReupdateBall = true;
				break;
			}
		}
	}

	if (shouldReupdateBall) {
		mBall->update(delta);
	}
}

void Game::draw() {
	SDL_SetRenderDrawColor(mRenderer, mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a);
	SDL_RenderClear(mRenderer);

	// Draw the score first so mBall can be drawn and seen in front of it
	mScoreKeeper->draw(mRenderer);

	for (const auto& paddle : mPaddles) {
		paddle->draw(mRenderer);
	}

	mBall->draw(mRenderer);

	SDL_RenderPresent(mRenderer);
}
