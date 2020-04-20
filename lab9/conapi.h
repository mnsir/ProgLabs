#pragma once

#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <conio.h>
#include <memory>
#include <functional>
#include "Student.h"
#include "tmanip.h"


static constexpr int ButtonHeight = 4;
static constexpr int ButtonWidth = 15;

int __cdecl isrus(int _C) {
	//принимает int из _getch() библиотеки conio.h. Правильно преобразуется в unsigned char.
	//setlocale не использовать!!!!
	// (А - Я) => 128 - 159
	// (а - п) => 160 - 175
	// (р - я) => 224 - 239
	// Ё - 240   ё - 241
	return _C >= 128 && _C <= 175 || _C >= 224 && _C <= 239;
}

struct XY {
	XY() = default;
	XY(const int& x, const int& y) : X(x), Y(y) {}
	int X;
	int Y;
};

class Console {
public:
	Console()
		: screenMaxX(1)
		, screenMaxY(1)
		, screenCenterX(1)
		, screenCenterY(1)
	{
		conOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (conOut == INVALID_HANDLE_VALUE) {
			lastError = GetLastError();
			return;
		}
		if (!GetConsoleMode(conOut, &conMode)) {
			lastError = GetLastError();
			return;
		}
	}

	void ActivateWindowsTerminal() {
		conMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(conOut, conMode)) {
			lastError = GetLastError();
			return;
		}
	}

	void FullScreen() {
		COORD coord = GetLargestConsoleWindowSize(conOut);
		SetConsoleScreenBufferSize(conOut, coord);
		SetConsoleDisplayMode(conOut, CONSOLE_FULLSCREEN_MODE, &coord);

		screenMaxX = coord.X;
		screenMaxY = coord.Y;
		screenCenterX = (coord.X + 1) / 2;
		screenCenterY = (coord.Y + 1) / 2;
		return;
	}

	void Title(const std::string& s) {
		TCHAR title[MAX_PATH];
		title[s.size()] = '\0';
		copy(s.begin(), s.end(), title);
		SetConsoleTitle(title);
	}

	const int& X() const { return screenMaxX; }
	const int& Y() const { return screenMaxY; }
	const int& CX() const { return screenCenterX; }
	const int& CY() const { return screenCenterY; }

private:
	// с доступом
	int screenMaxX;
	int screenMaxY;
	int screenCenterX;
	int screenCenterY;

	// без доступа
	DWORD lastError;
	HANDLE conIn;
	HANDLE conOut;
	DWORD conMode;
} con;		// Сразу же создаем экземпляр

using namespace std;

// ================================================================================================

enum class CR {
	Next,
	Prev,
	Apply,
	Cancel,
};

// ================================================================================================

class Control {
public:
	Control(
		const string& Caption,
		const int& X,
		const int& Y
	)
		: caption(Caption)
		, x(X)
		, y(Y)
	{
	}

	virtual void Skin2() {}
	virtual void Skin1() {}
	virtual void Skin3() {}
	virtual CR Action() { return CR::Next; }
	virtual void OnOK() {}
	virtual void Undo() {}

	int X() const { return x; }
	int Y() const { return y; }

protected:
	const string caption;
	const int x;
	const int y;
};

// ================================================================================================

class Edit : public Control {
public:
	Edit(
		const string& Caption,
		const int& X,
		const int& Y,
		string& Field,
		const size_t& charCount,
		int(__cdecl* IsRightChar)(int _C)
	)
		: Control(Caption, X, Y)
		, field(Field)
		, backup(Field)
		, maxSize(charCount)
		, isrightchar(IsRightChar)
	{
	}

	void Skin1() override {
		cout << set(x, y) << "\033[90m" << caption << ": " << erase_chars(maxSize) << field << "\033[0m";
	}

	void Skin2() override {
		cout << set(x, y) << "\033[1m" << caption << ": " << erase_chars(maxSize) << field << "\033[0m";
	}
	   
	void Skin3() override {
		cout << set(x, y) << "\033[93m" << caption << ": \033[1m" << erase_chars(maxSize) << field << "\033[0m";
	}

	CR Action() override {
		Skin2();
		string toBegin = set(x + caption.size() + 2U, y);
		for (int ch; ;) {
			if (_kbhit()) {
				ch = _getch();
				switch (ch) {
				case kb_down:
				case kb_right:
					if (field.size() > 0U && (int)field.back() == 224) field.pop_back();
				case kb_enter:
				case kb_tab:
					if (field.size() > 0U) {
						Skin1();
						return CR::Next;
					}
					Skin3();
					Sleep(150);
					Skin2();
					break;
				case kb_left:
				case kb_up:
					if (field.size() > 0U && (int)field.back() == 224) field.pop_back();
					if (field.size() > 0U) {
						Skin1();
						return CR::Prev;
					}
					Skin3();
					Sleep(150);
					Skin2();
					break;
				case kb_backspace:
					if (field.size() > 0U) field.pop_back();
					else {
						Skin3();
						Sleep(150);
					}
					Skin2();
					break;
				default:
					if (isrightchar(ch) && field.size() < maxSize) {
						field.push_back(static_cast<char>(ch));
						cout << toBegin << erase_chars(static_cast<int>(maxSize)) << field;
					}
					else break;
				}
			}
		}
	}
	void Undo() override { field = backup; }

protected:
	string& field;
	string backup;
	const size_t maxSize;
	int(__cdecl* isrightchar)(int ch);

};

// ================================================================================================

class Button : public Control {
public:
	Button(
		const string& Caption,
		const int& X,
		const int& Y
	)
		: Control(Caption, X, Y)
	{
	}

	void Skin1() override {
		cout << "\033[90m";
		DrawFrame();
		cout << "\033[0m" << set((12 - caption.size()) / 2 + x + 1, y + 1) << caption;
	}

	void Skin2() override {
		cout << "\033[1m";
		DrawFrame();
		cout << "\033[0m" << set((12 - caption.size()) / 2 + x + 1, y + 1) << caption;
	}

	void Skin3() override {
		cout << "\033[95m";
		DrawFrame();
		cout << set((12 - caption.size()) / 2 + x + 1, y + 1) << caption << "\033[0m";
	}

	virtual CR Action() override {
		for (int ch; ;)
			if (_kbhit()) {
				ch = _getch();
				switch (ch)
				{
				case kb_up:
				case kb_left:
					Skin1();
					return CR::Prev;
				case kb_down:
				case kb_right:
				case kb_tab:
					Skin1();
					return CR::Next;
				case kb_enter:
					Skin3();
					Sleep(300);
					Skin2();
					return CR::Apply;
				}
			}
	}

protected:
	void DrawFrame() {
		cout << set(x, y) << "\xda\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xbf"
			<< set(x, y + 1) << "\xb3            \xb3"
			<< set(x, y + 2) << "\xc0\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xd9";
	}
};

class CancelButton : public Button {
public:
	CancelButton(
		const string& Caption,
		const int& X,
		const int& Y
	)
		: Button(Caption, X, Y)
	{
	}

	CR Action() override {
		CR cr = Button::Action();
		if (cr == CR::Apply) return CR::Cancel;
		return cr;
	}
};


// ================================================================================================

template<class Enum>
class Selector : public Control {
public:
	Selector(
		const string& Caption,
		const int& X,
		const int& Y,
		Enum& Field
	)
		: Control(Caption, X, Y)
		, field(Field)
		, backup(Field)
		, maxSize(2U)
	{
	}

	void Skin1() override {
		UpdateMaxSize();
		cout << set(x, y) << "\033[90m" << caption << ": " << erase_chars(maxSize) << field << "\033[0m";
	}

	void Skin2() override {
		UpdateMaxSize();
		cout << set(x, y) << "\033[1m" << caption << ": <" << erase_chars(maxSize) << field << ">\033[0m";
	}

	void Skin3() override {
		UpdateMaxSize();
		cout << set(x, y) << "\033[1m" << caption << ": \033[93m<" << erase_chars(maxSize) << field << ">\033[0m";
	}

	CR Action() override {
		Skin2();
		string toBegin = set(x + caption.size() + 2U, y);
		size_t current = (
			static_cast<size_t>(field) < static_cast<size_t>(Enum::unknown)
			? static_cast<size_t>(field) : 0U);

		for (int ch; ;)
			if (_kbhit()) {
				ch = _getch();
				switch (ch)
				{
				case kb_up:
					Skin1();
					return CR::Prev;
				case kb_left:
					if (current == 0U) current = static_cast<size_t>(Enum::unknown) - 1U;
					else --current;
					field = static_cast<Enum>(current);
					Skin3();
					Sleep(300);
					Skin2();
					break;

				case kb_tab:
				case kb_right:
					if (current == static_cast<size_t>(Enum::unknown) - 1U) current = 0U;
					else ++current;
					field = static_cast<Enum>(current);
					Skin3();
					Sleep(300);
					Skin2();
					break;
				case kb_down:
				case kb_enter:
					Skin1();
					return CR::Next;
				}
			}
	}

	void Undo() override { field = backup; }

protected:
	void UpdateMaxSize() {
		size_t newSize = to_string(field).size() + 2U;
		maxSize = (newSize > maxSize ? newSize : maxSize);
	}

	Enum& field;
	Enum backup;
	size_t maxSize;
};

// ================================================================================================
class Table;

class TableUser {
public:
	virtual string GetHeader() const = 0;
	virtual size_t GetRecordCount() const = 0;
	virtual string GetRecord(const size_t& i) const = 0;
	//virtual void MakeFriend(Table* list) = 0;
};

class Table : public Control {
public:
	Table(
		const string& Caption,
		const int& X,
		const int& Y,
		shared_ptr<TableUser> User
	)
		: Control(Caption, X, Y)
		, current(0U)
	{
		MakeFriend(User);
		//User->MakeFriend(this);
	}

	void MakeFriend(shared_ptr<TableUser> User) {
		user = User;
	}

	void Skin1() override {
		if (user) {
			string header = user->GetHeader();
			size_t count = user->GetRecordCount();
			cout << left << set(x, y) << header;
			for (size_t i = 0; i < count; ++i) {
				cout << set(x, y + 1 + static_cast<int>(i)) << user->GetRecord(i);
			}
		}
	}

	void Skin2() override {
		if (user) {
			string header = user->GetHeader();
			size_t count = user->GetRecordCount();
			cout << left << set(x, y) << underline() << header << underline(false);
			for (size_t i = 0; i < count; ++i) {
				cout << set(x, y + 1 + static_cast<int>(i)) << user->GetRecord(i);
			}
		}
	}
	
	void Skin3() override {
		if (user) {
			string header = user->GetHeader();
			size_t count = user->GetRecordCount();
			cout << left << set(x, y) << underline() << header << underline(false);
			for (size_t i = 0; i < count; ++i) {
				cout << set(x, y + 1 + static_cast<int>(i)) 
					<< user->GetRecord(i);
			}
		}
	}

	//void Action() override {}

	//void OnOK() override {}
	//void Undo() override {}

	void Update() {
		Skin1();
	}
	size_t GetCurrent() const { return current; }

private:
	shared_ptr<TableUser> user;
	size_t current;
};

// ================================================================================================

class MenuUser {
public:
	virtual size_t GetMenuItemCount() const = 0;
	virtual string GetMenuItemName(const size_t& i) = 0;
	virtual void RunMenuItem(const size_t& i) = 0;
};

class Menu : public Control {
public:
	Menu(
		const string& Caption,
		const int& X,
		const int& Y,
		shared_ptr<MenuUser> User
	)
		: Control(Caption, X, Y)
		, width(Caption.size())
		, count(0U)
		, current(0)
	{
		MakeFriend(User);
	}

	void MakeFriend(shared_ptr<MenuUser> User) {
		user = User;
		if (user) {
			count = user->GetMenuItemCount();
			for (size_t i = 0; i < count; ++i) {
				auto size = static_cast<streamsize>(user->GetMenuItemName(i).size());
				if (size > width) width = size;
			}
		}
	}

	void Skin1() override {
		if (user) {
			cout << left << set(x, y) << setw(width) << caption;
			for (size_t i = 0; i < count; ++i) {
				cout << set(x, y + 1 + static_cast<int>(i))
					<< setw(width) << user->GetMenuItemName(i);
			}
		}
	}

	void Skin2() override {
		if (user) {
			cout << left << set(x, y) << underline << setw(width) << caption << underline(false);
			for (size_t i = 0; i < count; ++i) {
				cout << set(x, y + 1 + static_cast<int>(i))
					<< setw(width) << user->GetMenuItemName(i);
			}
		}
	}

	void Skin3() override {
		if (user) {
			cout << left << set(x, y) << underline << setw(width) << caption << underline(false);
			for (size_t i = 0; i < count; ++i) {
				cout << set(x, y + 1 + static_cast<int>(i))
					<< (i == current ? invert() : "")
					<< setw(width)
					<< user->GetMenuItemName(i)
					<< (i == current ? invert(false) : "");
			}
		}
	}

	CR Action() override {
		if (user) {
			Skin3();
			int last = static_cast<int>(count - 1U);
			for (int ch; ;)
				if (_kbhit()) {
					ch = _getch();
					switch (ch)
					{
					case kb_up:
					case kb_left:
						if (current == 0) current = last;
						else --current;
						Skin3();
						break;

					case kb_tab:
					case kb_right:
					case kb_down:
						if (current == last) current = 0;
						else ++current;
						Skin3();
						break;

					case kb_enter:
						user->RunMenuItem(current);
						return CR::Apply;
					}
				}
		}
	}

	void OnOK() override {}
	void Undo() override {}

protected:
	shared_ptr<MenuUser> user;
	streamsize width;
	size_t count;
	int current;
};

// ================================================================================================

class Taber : public Control {
public:
	explicit Taber(
		const string& Caption,
		const size_t& X0,
		const size_t& Y0,
		const size_t& X1,
		const size_t& Y1
	)
		: Control(Caption, X0, Y0)
		, x1(X1)
		, y1(Y1)
		, current(0U)
	{
	}

	~Taber() {
		Erase();
	}

	void Add(const shared_ptr<Control>& control) {
		controls.push_back(control);
	}

	void DrawControls() {
		for (const auto& p : controls)
			p->Skin1();
		controls[current]->Skin2();
	}

	void Erase() const {
		for (int xi = x; xi <= x1; ++xi) {
			for (int yi = y; yi <= y1; ++yi) {
				cout << set(xi, yi) << ' ';
			}
		}
	}

	void Move(const CR& cr) {
		if (cr == CR::Next) ++current;
		else if (cr == CR::Prev) --current;
	}

	void Cycle() {

		while (true) {
			CR cr = controls[current]->Action();
			switch (cr)
			{
			case CR::Next:
				++current;
				break;
			case CR::Prev:
				--current;
				break;
			case CR::Apply:
				return;
			case CR::Cancel:
				for (auto& con : controls)
					con->Undo();
				return;
			default:
				break;
			}
		}

		bool repeat = true;
		for (int ch = 0; repeat; ) {

			if (_kbhit()) {
				ch = _getch();
				switch (ch)
				{
				case kb_up:
				case kb_left:
					controls[current]->Skin1();
					if (current == 0U) current = controls.size() - 1U;
					else --current;
					controls[current]->Skin2();
					break;

				case kb_tab:
				case kb_right:
				case kb_down:
					controls[current]->Skin1();
					if (current == controls.size() - 1U) current = 0U;
					else ++current;
					controls[current]->Skin2();
					break;

				case kb_enter:
					controls[current]->Action();

					Result button = GetPushedButton();
					switch (button)
					{
					case Result::Ok:
						for (const auto& item : controls) {
							item->OnOK();
						}
						repeat = false;
						break;

					case Result::Cancel:
						for (const auto& item : controls) {
							item->Undo();
						}
						repeat = false;
						break;

					case Result::None:
					default:
						break;
					}
					
				}
			}
		}
	}

	CR Action() override {
		DrawFrame();
		if (!controls.empty())
		{
			DrawControls();
			Cycle();
		}
		return CR::Next;
	}

protected:
	enum class Result : int {
		None = 0,
		Ok,
		Cancel,
	};

	virtual Result GetPushedButton() { return Result::None; }

	void DrawFrame() {
		for (int i = x; i < x1; ++i)
			cout << set(i, y) << '\xcd' << set(i, y1) << '\xcd'
			<< set(i, y + 2) << '\xcd';
		for (int i = y; i < y1; ++i)
			cout << set(x, i) << '\xba' << set(x1, i) << '\xba';
		cout << set(x, y) << '\xc9' << set(x1, y) << '\xbb' << set(x1, y1) << '\xbc' << set(x, y1) << '\xc8'
			<< set(x, y + 2) << '\xcc' << set(x1, y + 2) << '\xb9'
			<< set((x + x1 - caption.size()) / 2, y + 1) << caption;
	}

	size_t current;
	vector<shared_ptr<Control>> controls;

	int x1;
	int y1;
};

// ================================================================================================

/*class TaberOkCancel : public Taber {
public:
	explicit TaberOkCancel(
		const string& Caption,
		const size_t& OffsetX,
		const size_t& OffsetY,
		const size_t& ScrX,
		const size_t& ScrY
	)
		: Taber(Caption, OffsetX, OffsetY, ScrX, ScrY)
	{
		controls.push_back(make_shared<Button>(
			"OK"_s,
			x1 - ButtonWidth - ButtonWidth,
			y1 - ButtonHeight + 1
			));

		controls.push_back(make_shared<Button>(
			"Отмена"_s,
			x1 - ButtonWidth,
			y1 - ButtonHeight + 1
			));

		current = 2U;
	}

protected:
	enum class ButtonType : size_t {
		Ok,
		Cancel,
	};

	Result GetPushedButton() override {
		ButtonType t = static_cast<ButtonType>(current);
		switch (t)
		{
		case ButtonType::Ok:		return Result::Ok;
		case ButtonType::Cancel:	return Result::Cancel;
		default:					return Result::None;
		}
	}
};*/
