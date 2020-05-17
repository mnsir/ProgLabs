#pragma once

#include <Windows.h>
#include <string>

// ================================================================================================
// Console

class Console {
public:
	Console()
		: screenMaxX(1)
		, screenMaxY(1)
		, screenCenterX(1)
		, screenCenterY(1)
	{
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdout == INVALID_HANDLE_VALUE) {
			lastError = GetLastError();
			return;
		}
		if (!GetConsoleMode(hStdout, &mode)) {
			lastError = GetLastError();
			return;
		}
		GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	}

	void SetCursorPosition(const int& x, const int& y) {
		COORD coord;
		coord.X = x;
		coord.Y = y;
		BOOL result = SetConsoleCursorPosition(hStdout, coord);
	}

	void ActivateWindowsTerminal() {
		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hStdout, mode)) {
			lastError = GetLastError();
			return;
		}
	}

	void FullScreen() {
		COORD coord = GetLargestConsoleWindowSize(hStdout);
		SetConsoleScreenBufferSize(hStdout, coord);
		SetConsoleDisplayMode(hStdout, CONSOLE_FULLSCREEN_MODE, &coord);

		screenMaxX = coord.X;
		screenMaxY = coord.Y;
		screenCenterX = (coord.X + 1) / 2;
		screenCenterY = (coord.Y + 1) / 2;
		return;
	}

	void SetTitle(const std::string& s) {
		TCHAR title[MAX_PATH];
		title[s.size()] = '\0';
		copy(s.begin(), s.end(), title);
		SetConsoleTitle(title);
	}

	int MX() const { return screenMaxX; }
	int MY() const { return screenMaxY; }
	int CX() const { return screenCenterX; }
	int CY() const { return screenCenterY; }

private:
	// с доступом
	int screenMaxX;
	int screenMaxY;
	int screenCenterX;
	int screenCenterY;

	// вспомогательные
	DWORD temp;

	// без доступа
	DWORD lastError;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE hStdin;
	HANDLE hStdout;
	DWORD mode;
};