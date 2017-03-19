#include "Game.h"
#include <SDL.h>
#include <iostream>

void Game::processEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				onQuit();
				break;
			case SDL_KEYDOWN:
				onKeyDown(&e.key);
				break;
			case SDL_KEYUP:
				onKeyUp(&e.key);
				break;
			case SDL_MOUSEBUTTONDOWN:
				onMouseDown(&e.button);
				break;
			case SDL_MOUSEBUTTONUP:
				onMouseUp(&e.button);
				break;
			case SDL_MOUSEMOTION:
				onMouseMotion(&e.motion);
				break;
			default:
				break;
		}
	}
}

void Game::onQuit() {
	std::cout << "Application was told to quit" << std::endl;

	mShouldQuit = true;
}

void Game::onKeyDown(const SDL_KeyboardEvent* kbe) {
	switch (kbe->keysym.sym) {
		case SDLK_ESCAPE:
			std::cout << "User pressed Escape. Quitting." << std::endl;
			mShouldQuit = true;
			break;
		case SDLK_r:
			unload();
			load();
			break;
		case SDLK_COMMA: {
				int scores[] = { 1, 0 };
				mScoreKeeper->message(MessageId::UPDATE_SCORE, scores);
			}
			break;
		case SDLK_PERIOD: {
				int scores[] = { 0, 1 };
				mScoreKeeper->message(MessageId::UPDATE_SCORE, scores);
			}
			break;
	}
}

void Game::onKeyUp(const SDL_KeyboardEvent* kbe) {
}

void Game::onMouseDown(const SDL_MouseButtonEvent* mbe) {
}

void Game::onMouseUp(const SDL_MouseButtonEvent* mbe) {
}

void Game::onMouseMotion(const SDL_MouseMotionEvent* mme) {
}