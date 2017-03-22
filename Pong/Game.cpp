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
	// Prepare arguments for the first couple of messages
	const int screenDimens[] = { mScreenWidth, mScreenHeight };
	int ballScreenXBoundsStates[2];

	// First handle where the paddle should update to
	for (const auto& paddle: mPaddles) {
		// Set the paddles' velocities based on the key states
		if (paddle->message(MessageId::SET_Y_VELOCITY, mKeys)) {
			paddle->update(delta);
			// Handle paddles going off the top/bottom of screen after the update
			paddle->message(MessageId::Y_OUT_OF_SCREEN_BOUNDS, screenDimens);
		}
	}

	// Next, update the Ball and try to perform screen bounds handling on it
	mBall->update(delta);
	bool shouldReupdateAfterVelocityChange = false;

	if (mBall->message(MessageId::Y_OUT_OF_SCREEN_BOUNDS, screenDimens)) {
		shouldReupdateAfterVelocityChange = true;
	} else if (mBall->message(MessageId::X_OUT_OF_SCREEN_BOUNDS, screenDimens, ballScreenXBoundsStates)) {
		mScoreKeeper->message(MessageId::UPDATE_SCORE, ballScreenXBoundsStates);
	} else {
		int angle;

		for (const auto& paddle : mPaddles) {
			if (paddle->message(MessageId::HANDLE_COLLISION, mBall, &angle)) {
				// Update it's new velocity based on that collision, using the newly assigned angle value
				mBall->message(MessageId::PADDLE_BOUNCE, &angle);
				shouldReupdateAfterVelocityChange = true;
				break;
			}
		}
	}

	if (shouldReupdateAfterVelocityChange) {
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
