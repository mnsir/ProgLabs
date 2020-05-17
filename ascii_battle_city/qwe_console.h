#pragma once

#include <Windows.h>
#include <string>

// ================================================================================================
// Console

class Console {
public:
	Console() {
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hStdout == INVALID_HANDLE_VALUE) {
			lastError = GetLastError();
			return;
		}
		if (!GetConsoleMode(hStdout, &mode)) {
			lastError = GetLastError();
			return;
		}
	}

	bool SetCursorPosition(const short& x, const short& y) { return SetConsoleCursorPosition(hStdout, {x, y}); }

	void ActivateWindowsTerminal() {
		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hStdout, mode)) {
			lastError = GetLastError();
			return;
		}
	}

	void SetFontSize(const short& x, const short& y) {
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = x;                   // Width of each character in the font
		cfi.dwFontSize.Y = y;                  // Height
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		wcscpy_s(cfi.FaceName, L"Consolas"); // Choose your font
		SetCurrentConsoleFontEx(hStdout, FALSE, &cfi);
	}

	bool UpdateScreenBufferSize() { return GetConsoleScreenBufferInfo(hStdout, &csbiInfo); }
	short GetWidth() const { return csbiInfo.dwSize.X; }
	short GetHeight() const { return csbiInfo.dwSize.Y; }

	void FullScreen() {
		COORD coord = GetLargestConsoleWindowSize(hStdout);
		SetConsoleDisplayMode(hStdout, CONSOLE_FULLSCREEN_MODE, &coord);
		GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
		SetConsoleScreenBufferSize(hStdout, csbiInfo.dwMaximumWindowSize);
	}

	void SetTitle(const char* title) { SetConsoleTitleA(title); }

	bool Write(const char* str) { return WriteConsoleA(hStdout, str, strlen(str), &temp, NULL); }

	void WriteChar(const char& ch, const short& x = -1, const short& y = -1, const size_t& count = 1) {
		if (x < 0 || y < 0) return; // TODO автоопределение
		FillConsoleOutputCharacterA(hStdout, ch, count, { x, y }, &temp);
	}

	// ===============================================================================================
	// Для игры

	void DrawFrame() {
		GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
		FillConsoleOutputCharacterA(hStdout, '\xCD', csbiInfo.dwSize.X, { 0, 0 }, &temp);
		FillConsoleOutputCharacterA(hStdout, '\xCD', csbiInfo.dwSize.X, { 0, 2 }, &temp);
		FillConsoleOutputCharacterA(hStdout, '\xCD', csbiInfo.dwSize.X, { 0, csbiInfo.dwSize.Y - 1 }, &temp);
		for (short i = 0i16; i < csbiInfo.dwSize.Y; ++i) {
			FillConsoleOutputCharacterA(hStdout, '\xBA', 1, { 0, i }, &temp);
			FillConsoleOutputCharacterA(hStdout, '\xBA', 1, { csbiInfo.dwSize.X - 1, i }, &temp);
		}
		FillConsoleOutputCharacterA(hStdout, '\xC9', 1, { 0, 0 }, &temp);
		FillConsoleOutputCharacterA(hStdout, '\xCC', 1, { 0, 2 }, &temp);
		FillConsoleOutputCharacterA(hStdout, '\xBB', 1, { csbiInfo.dwSize.X - 1, 0 }, &temp);
		FillConsoleOutputCharacterA(hStdout, '\xB9', 1, { csbiInfo.dwSize.X - 1, 2 }, &temp);
		FillConsoleOutputCharacterA(hStdout, '\xC8', 1, { 0, csbiInfo.dwSize.Y - 1 }, &temp);
		FillConsoleOutputCharacterA(hStdout, '\xBC', 1, { csbiInfo.dwSize.X - 1, csbiInfo.dwSize.Y - 1 }, &temp);
	}

private:
	// вспомогательные
	DWORD temp;

	// без доступа
	DWORD lastError;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	HANDLE hStdin;
	HANDLE hStdout;
	DWORD mode;
};