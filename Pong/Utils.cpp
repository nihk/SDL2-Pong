#pragma once

#include "Utils.h"
#include <cstdlib>

int RandomHelpers::getRandomNumberWithinRange(int min, int max) {
	return min + rand() % static_cast<int>(max - min + 1);
}

int RandomHelpers::getRandomNumberWithinTwoRanges(int min1, int max1, int min2, int max2) {
	int num1 = getRandomNumberWithinRange(min1, max1);
	int num2 = getRandomNumberWithinRange(min2, max2);

	return rand() % 2 == 0 ? num1 : num2;
}

void RandomHelpers::seedRandomNumberGenerator(int seed) {
	srand(seed);
}
