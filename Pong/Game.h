#pragma once

#include "Paddle.h"
#include "Ball.h"
#include "TwoPlayerScoreKeeper.h"
#include <vector>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;

class Game {
public:
	Game();

	bool run();

private:
	int mScreenWidth;
	int mScreenHeight;
	bool mShouldQuit;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	SDL_Color mBackgroundColor;
	const uint8_t* mKeys;

	std::vector<Entity*> mPaddles;
	Entity* mBall;
	TwoPlayerScoreKeeper* mScoreKeeper;

	bool initialize();
	void load();
	void unload();
	void shutdown();
	void onQuit();

	void processEvents();
	void onKeyDown(const SDL_KeyboardEvent* kbe);
	void onKeyUp(const SDL_KeyboardEvent* kbe);
	void onMouseDown(const SDL_MouseButtonEvent* mbe);
	void onMouseUp(const SDL_MouseButtonEvent* mbe);
	void onMouseMotion(const SDL_MouseMotionEvent* mme);

	void update(float delta);
	void draw();
};

