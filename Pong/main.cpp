/*
* Nick Rose
* Assignment 2 - Game Engineering
* March 27th, 2017
*/

#include <iostream>

#include "Game.h"

int main(int argc, char** argv) {
	Game game;

	std::cout << "Welcome to Assignment 2 - Pong." << std::endl;

	if (!game.run()) {
		std::cerr << "*** Something horrible has happened" << std::endl;
		return 1;
	}

	std::cout << "Goodbye" << std::endl;

	return 0;
}
