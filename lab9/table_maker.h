#pragma once

#include <iostream>
#include <memory>
#include "tmanip.h"

void SetCaption(const std::string& s, const int& X, const int& Y) {
	for (int i = 1; i <= X; ++i) {
		if (i == 1) std::cout << '\xc9';
		else if (i == X) std::cout << '\xbb' << down_line() << '\xba';
		else std::cout << '\xcd';
	}
	std::cout << setcol(X) << '\xba' << down_line();
	for (int i = 1; i <= X; ++i) {
		if (i == 1) std::cout << '\xc8';
		else if (i == X) std::cout << '\xbc';
		else std::cout << '\xcd';
	}
	int x = (X + 1 - static_cast<int>(s.size())) / 2;
	std::cout << set(x, 2) << s << down_line(2);
}

