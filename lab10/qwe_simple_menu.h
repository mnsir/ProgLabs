#pragma once

#include <conio.h>
#include <iostream>
#include <string>

// Простое меню

// ВАЖНО АКТИВИРОВАТЬ РЕЖИМ ТЕРМИНАЛА!!!

// Как пользоваться:

//while (true) {
//	switch (MenuResult("Select", { "One", "Two", "Three" })) {
//	case 0:
//		cout << "zero\n";
//		break;
//	case 1:
//		cout << "one\n";
//		break;
//	case 2:
//		cout << "two\n";
//		break;
//	case 3:
//		cout << "three\n";
//		break;
//	}
//}

int MenuResult(const std::string& caption, const std::initializer_list<std::string>& commands) {
	int res = 0;
	size_t charCount = caption.size() + 1 + commands.size() * 12;
	std::cout << caption << ':';
	int i = 1;
	for (const auto& command : commands) {
		std::cout << "   \033[96m[кн." << i++ << "]\033[0m - " << command;
		charCount += command.size();
	}
	while (true) {
		if (_kbhit()) {
			res = _getch() - 48;
			if (0 < res && res <= static_cast<int>(commands.size())) break;
		}
	}
	std::cout << '\r' << std::string(charCount, ' ') << '\r' << "[" << *(commands.begin() + res - 1) << "]\n";
	return res;
}