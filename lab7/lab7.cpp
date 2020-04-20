#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stack>
#include <tuple>
#include <set>
#include <chrono>

using namespace std;

// VT100
#define SET(x, y)		"\033[" << (y) << ';' << (x) << "H"
#define UNDERLINE		"\033[4m"
#define NOUNDERLINE		"\033[24m"
#define NEGATIVE		"\033[7m"
#define NONEGATIVE		"\033[27m"
#define HIDE			"\033[?25l"
#define SHOW			"\033[?25s"
#define SAVE			"\033[s"
#define LOAD			"\033[u"
#define UP(n)			"\033[" << (n) << "A"
#define DOWN(n)			"\033[" << (n) << "B"
#define RIGHT(n)		"\033[" << (n) << "C"
#define LEFT(n)			"\033[" << (n) << "D"
#define ERASE_SCREEN(n)	"\033[" << (n) << "J"
#define ERASE_CHAR(n)	"\033[" << (n) << "X"

// Для _kbhit
constexpr int KB_0 = 48;
constexpr int KB_1 = 49;
constexpr int KB_2 = 50;
constexpr int KB_3 = 51;
constexpr int KB_4 = 52;
constexpr int KB_5 = 53;
constexpr int KB_6 = 54;
constexpr int KB_7 = 55;
constexpr int KB_8 = 56;
constexpr int KB_9 = 57;
constexpr int KB_Enter = 13;
constexpr int KB_ESC = 27;
constexpr int KB_TAB = 9;
constexpr int KB_Space = 32;
constexpr int KB_Backspace = 8;
constexpr int KB_Up = 72;
constexpr int KB_Down = 80;
constexpr int KB_Left = 75;
constexpr int KB_Right = 77;

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

	void Write(const string& s) {
		WriteFile(hStdout, s.c_str(), s.size(), &temp, 0);
	}

	string Read() {
		CHAR buffer[256];
		ReadFile(hStdin, buffer, 255, &temp, 0);
		return string(buffer);
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
} CONSOLE;					// Экземпляр!!

// ================================================================================================
// Eraser
// Хранит координаты начала надписи и количество символов

class Eraser {
public:
	void Add(const int& X, const int& Y, const size_t& Count) { s.push(Record(X, Y, Count)); }

	void ClearLast(size_t records = 1) {
		while (records-- > 0) {
			Record& rec = s.top();
			cout << SET(rec.X, rec.Y) ERASE_CHAR(rec.Count);
			s.pop();
		}
	}

	void Clear() {
		while (!s.empty()) {
			Record& rec = s.top();
			cout << SET(rec.X, rec.Y) ERASE_CHAR(rec.Count);
			s.pop();
		}
	}

private:
	struct Record {
		Record(const int& x, const int& y, const size_t& count)
			: X(x), Y(y), Count(count) {}
		int X;
		int Y;
		size_t Count;
	};

	stack<Record> s;
} ERASER;					// Экземпляр!!

// ================================================================================================
// Caption

enum class Style : size_t {
	Default,
	Underline,
	Negative
};

// Любая надпись. Удаляется через ERASER
void Caption(const int& x, const int& y, const string& caption, const Style& style = Style::Default) {
	switch (style) {
	case Style::Underline:
		cout << SET(x, y) UNDERLINE << caption << NOUNDERLINE;
		break;
	case Style::Negative:
		cout << SET(x, y) NEGATIVE << caption << NONEGATIVE;
		break;
	default:
		cout << SET(x, y) << caption;
		break;
	}
	ERASER.Add(x, y, caption.size());
}

// ================================================================================================
// Костыли

// compile time рекурсивный подсчет длины строки
constexpr size_t length(const char* c, size_t counter = 1U) {
	if (*c == '\0') return counter;
	return length(c + 1, counter + 1);
}

int __cdecl isrus(int _C) {
	//принимает int из _getch() библиотеки conio.h. Правильно преобразуется в unsigned char.
	//setlocale не использовать!!!!
	// (А - Я) => 128 - 159
	// (а - п) => 160 - 175
	// (р - я) => 224 - 239
	// Ё - 240   ё - 241
	return _C >= 128 && _C <= 175 || _C >= 224 && _C <= 239;
}

// Перевод кроказябры в рус
char Normalize(const char& letter) {
	const char from[2] = { letter, '\0' };
	char to[2];
	OemToCharA(from, to);
	return to[0];
}

string Today() {
	constexpr size_t size = 20U;
	CHAR date[size];
	GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "dd-MMMM-yyyy", date, size);
	return date;
}

enum class DayPeriod : bool {
	AM,	// До полудня
	PM	// После полудня
};

ostream& operator<<(ostream& os, const DayPeriod& time) { return os << static_cast<bool>(time); }
istream& operator>>(istream& is, DayPeriod& time) {
	bool temp;
	is >> temp;
	time = static_cast<DayPeriod>(temp);
	return is;
}

DayPeriod GetDayPeriod() {
	using namespace chrono;
	constexpr hours moscowGTMOffset(3);	// Смещение по московскому времени
	constexpr hours noonTime(12);		// Время полудня
	constexpr hours hoursPerDay(24);	// В сутках
	const hours hoursNow = duration_cast<hours>(system_clock::now().time_since_epoch()) % hoursPerDay;
	if (hoursNow < noonTime - moscowGTMOffset) return DayPeriod::AM;
	return DayPeriod::PM;
}

// ================================================================================================
// Getter

template<class Type>
Type Get(istream& is) {
	Type temp;
	is >> temp;
	return temp;
}

// ================================================================================================
// Input

string Input(
	string s,
	int(__cdecl* IsRightChar)(int _C),
	const size_t& minChars,
	const size_t& maxChars,
	const int& x,
	const int& y
) {
	cout << SET(x, y) << s;
	int ch;
	setlocale(0, "C");
	while (true) {
		if (_kbhit()) {
			ch = _getch();
			switch (ch) {
			case KB_Enter:
				if (s.size() >= minChars && s.size() <= maxChars) {
					cout << SET(x, y) ERASE_CHAR(s.size());
					setlocale(0, "");
					return s;
				}
				break;
			case KB_Backspace:
				if (s.size() > 0) {
					s.pop_back();
					cout << "\b \b";
				}
				break;
			case 224:
				// Если 224 и еще что то есть в _kbhit - значит нажата стрелка
				if (_kbhit()) ch = _getch();
				break;
			}

			if (IsRightChar(ch) && s.size() < maxChars) {
				char c = static_cast<char>(ch);
				s.push_back(Normalize(c));
				cout << c;
			}
		}
	}
}

// Для ФИО
string InputName(const string& s, const int& x, const int& y) {
	Caption(2, CONSOLE.MY() - 4, "Введите с клавиатуры РУССКИЕ буквы..");
	Caption(2, CONSOLE.MY() - 3, "Возможно потребуется сменить раскладку клавиатуры.");
	Caption(2, CONSOLE.MY() - 2, "BACKSPACE - стереть последний символ, ENTER - ввод");
	Caption(2, CONSOLE.MY() - 1, "Поле не должно быть пустым. Максимальная длина - 20 символов.");

	string str = Input(s, isrus, 2, 20, x, y);
	for (auto& c : str)
		c = static_cast<char>(tolower(c));
	str[0] = static_cast<char>(toupper(str[0]));
	ERASER.ClearLast(4U);
	return str;
}

// Для № группы
string InputGroup(const string& s, const int& x, const int& y) {
	Caption(2, CONSOLE.MY() - 3, "Введите с клавиатуры 4 цифры..");
	Caption(2, CONSOLE.MY() - 2, "BACKSPACE - стереть последний символ, ENTER - ввод");
	Caption(2, CONSOLE.MY() - 1, "Поле не должно быть пустым.");
	string group = Input(s, isdigit, 4, 4, x, y);
	ERASER.ClearLast(3U);
	return group;
}

// ================================================================================================
// Selector
// Шаблон необходим чтоб принимать std::initializer_list<const char*> и std::vector<std::string> 

template<class Container>
size_t BasicSelect(const Container& items, const int& x, const int& y, size_t selected = 0U) {
	size_t captions = 0U;
	Caption(2, CONSOLE.MY() - 1, "ВВЕРХ, ВНИЗ - управлять, ENTER - ввод");
	auto func = [&]() {
		for (size_t i = 0U; i < items.size(); ++i) {
			Caption(x, y + i, *(items.begin() + i), (i == selected ? Style::Negative : Style::Default));
			++captions;
		}
	};

	func();
	while (true) {
		if (_kbhit()) {
			int ch = _getch();
			switch (ch) {
			case KB_Enter:
				ERASER.ClearLast(captions);
				ERASER.ClearLast();
				cout << SET(x, y);
				return selected;
			case 224:
				ch = _getch();
				switch (ch) {
				case KB_Up:
				case KB_Left:
					if (selected == 0U) selected = items.size() - 1U;
					else --selected;
					func();
					break;
				case KB_Right:
				case KB_Down:
					if (selected == items.size() - 1U) selected = 0U;
					else ++selected;
					func();
					break;
				}
				break;
			}
		}
	}
}

// Общий шаблон для перечислений НЕ ИСПОЛЬЗУЕТСЯ
// Для каждого перечисления существует соответствующая специализация шаблона

template<class T> T Select(const int&, const int&) { return T(); }

// ================================================================================================
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ПЕРЕЧИСЛЕНИЯ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// std::initializer_list<const char*> вариантов
// Перечисление вариантов
// Специализация шаблона Select
// Перегрузка operator<<
// Перегрузка to_string

// ------------------------------------------------------------------------------------------------
// TSex
// Пол

auto SEX = {
	"мужской",
	"женский"
};

enum class TSex : size_t {
	male,
	female,
	unknown,
};

template<>
TSex Select(const int& x, const int& y) {
	Caption(x, y, " Выберите ПОЛ студента: ", Style::Underline);
	return static_cast<TSex>(BasicSelect(SEX, x + 1, y + 1));
}

string to_string(const TSex& sex) { return *(SEX.begin() + static_cast<size_t>(sex)); }

ostream& operator<<(ostream& os, const TSex& sex) { return os << to_string(sex); }

// ------------------------------------------------------------------------------------------------
// TForm
// Форма обучения

auto FORM = {
	"очная",
	"вечерняя",
	"заочная",
};

enum class TForm : size_t {
	FullTime,
	Evening,
	Corespondence,
};

template<>
TForm Select(const int& x, const int& y) {
	Caption(x, y, " Выберите ФОРМУ ОБУЧЕНИЯ студента: ", Style::Underline);
	return static_cast<TForm>(BasicSelect(FORM, x + 1, y + 1));
}

string to_string(const TForm& form) { return *(FORM.begin() + static_cast<size_t>(form)); }

ostream& operator<<(ostream& os, const TForm& form) { return os << to_string(form); }

// ------------------------------------------------------------------------------------------------
// TMark
// Оценка

auto MARK = {
	"отл",
	"хор",
	"уд",
	"неуд"
};

enum class TMark : size_t {
	m5,
	m4,
	m3,
	m2,
};

template<>
TMark Select(const int& x, const int& y) {
	Caption(x, y, " Выберите ОЦЕНКУ: ", Style::Underline);
	return static_cast<TMark>(BasicSelect(MARK, x + 1, y + 1));
}

string to_string(const TMark& mark) { return *(MARK.begin() + static_cast<size_t>(mark)); }

ostream& operator<<(ostream& os, const TMark& mark) { return os << to_string(mark); }

// ------------------------------------------------------------------------------------------------
// Field
// Поля таблицы

auto FIELD = {
	"ID",
	"Фамилия",
	"Имя",
	"Отчество",
	"Пол",
	"Форма",
	"Гр.",
	"№",
	"ВВвИТ",
	"Матан",
	"А_и_Г",
	"Истор",
	"Ин_Яз",
	"Прог1",
	"Прог2",
	"Филос",
	"Изменен"
};

enum class Field : size_t {
	ID,
	LastName,
	FirstName,
	SecondName,
	Sex,
	Form,
	GroupID,
	ListNumber,
	Exam0,
	Exam1,
	Exam2,
	Credit0,
	Credit1,
	Credit2,
	Credit3,
	Credit4,
	EditDate,
};

template<>
Field Select(const int& x, const int& y) {
	Caption(x, y, " Выберите поле из спиcка: ", Style::Underline);
	Field selected = static_cast<Field>(BasicSelect(FIELD, x + 1, y + 1));
	ERASER.ClearLast();
	return selected;
}

constexpr const char* to_c_str(const Field& field) { return *(FIELD.begin() + size_t(field)); }

string to_string(const Field& field) { return *(FIELD.begin() + static_cast<size_t>(field)); }

ostream& operator<<(ostream& os, const Field& field) { return os << to_string(field); }

// ================================================================================================
// Student

struct Student {
	Student() = default;

	size_t		ID;			// Уникальный идентификатор
	string		LastName;	// Фамилия
	string		FirstName;	// Имя
	string		SecondName;	// Отчество
	TSex		Sex;		// Пол
	TForm		Form;		// Форма обучения
	string		GroupID;	// Номер группы
	size_t		ListNumber;	// Номер в списке группы
							// Оценки:
	TMark		Exam[3];	// экзамены
	TMark		Credit[5];	// зачеты
							// Время редактирования:
	string		EditDate;	// Дата в виде строки
	DayPeriod	EditTime;	// Время ДО или ПОСЛЕ полудня (по ТЗ)

	string FullName() const {
		return LastName.empty() ? "" : "гр. " + GroupID + ' '
			+ LastName + ' ' + FirstName[0] + ". " + SecondName[0];
	}

	// Получить информацию, хранящуюся в поле, сторкой
	string GetField(const size_t& i) const {
		switch (i) {
		case 0U:	return to_string(ID);
		case 1U:	return LastName;
		case 2U:	return FirstName;
		case 3U:	return SecondName;
		case 4U:	return to_string(Sex);
		case 5U:	return to_string(Form);
		case 6U:	return GroupID;
		case 7U:	return to_string(ListNumber);
		case 8U:	return to_string(Exam[0]);
		case 9U:	return to_string(Exam[1]);
		case 10U:	return to_string(Exam[2]);
		case 11U:	return to_string(Credit[0]);
		case 12U:	return to_string(Credit[1]);
		case 13U:	return to_string(Credit[2]);
		case 14U:	return to_string(Credit[3]);
		case 15U:	return to_string(Credit[4]);
		case 16U:	return EditDate;
		default:	return "error";
		}
	}

	//Выбирает поле, которое необходимо изменить
	void Change(const Field& field, int x, int y) {
		string title = "Поле " + to_string(field) + ": ";
		Caption(x, y, title);
		switch (field) {
		case Field::ID:																		break;
		case Field::LastName:	LastName	= InputName(LastName, x + title.size(), y);		break;
		case Field::FirstName:	FirstName	= InputName(FirstName, x + title.size(), y);	break;
		case Field::SecondName:	SecondName	= InputName(SecondName, x + title.size(), y);	break;
		case Field::Sex:		Sex			= Select<TSex>(x, y + 2);						break;
		case Field::Form:		Form		= Select<TForm>(x, y + 2);						break;
		case Field::GroupID:	GroupID		= InputGroup(GroupID, x + title.size(), y);		break;
		case Field::ListNumber:																break;
		case Field::Exam0:		Exam[0]		= Select<TMark>(x, y + 2);						break;
		case Field::Exam1:		Exam[1]		= Select<TMark>(x, y + 2);						break;
		case Field::Exam2:		Exam[2]		= Select<TMark>(x, y + 2);						break;
		case Field::Credit0:	Credit[0]	= Select<TMark>(x, y + 2);						break;
		case Field::Credit1:	Credit[1]	= Select<TMark>(x, y + 2);						break;
		case Field::Credit2:	Credit[2]	= Select<TMark>(x, y + 2);						break;
		case Field::Credit3:	Credit[3]	= Select<TMark>(x, y + 2);						break;
		case Field::Credit4:	Credit[4]	= Select<TMark>(x, y + 2);						break;
		case Field::EditDate:																break;
		default:																			break;
		}
		EditDate = Today();
		EditTime = GetDayPeriod();
		ERASER.ClearLast();
	}

	// Рейтинг студента
	size_t Ratio() const {
		size_t ratio = 0U;
		for (const auto& mark : Exam)
			ratio += static_cast<size_t>(mark);
		for (const auto& mark : Credit)
			ratio += static_cast<size_t>(mark);
		return ratio;	// На количество предметов не делим, т к нет смысла, да и операция дорогая
	}
};

// Перегрузил для set
bool operator<(const shared_ptr<Student>& lhs, const shared_ptr<Student>& rhs) {
	return tie(lhs->LastName, lhs->FirstName, lhs->SecondName) 
		< tie(rhs->LastName, rhs->FirstName, rhs->SecondName);
}

// ================================================================================================
// Database
// Класс базы данных. Комструктор читает файл, деструктор его сохраняет.
// Публичные методы Add и Edit позволяют создавать и редактировать записи,
// остальные фильтруют, сортируют и выводят их в соответствии с ТЗ

class Database {
public:
	Database() : base("base.txt")
		, recordCount(0U)
		, lastID(0U)
		, setws(vector<streamsize>(FIELD.size(), 0))
	{
		ifstream in(base);
		if (in) {
			in >> lastID >> recordCount;
			for (size_t i = 0; i < FIELD.size(); ++i) {
				streamsize temp;
				in >> temp;
				setws[i] = temp;
			}
			for (size_t i = 0; i < recordCount; ++i) {
				Student s;
				in >> s.ID >> s.LastName >> s.FirstName >> s.SecondName;
				s.Sex = static_cast<TSex>(Get<size_t>(in));
				s.Form = static_cast<TForm>(Get<size_t>(in));
				in >> s.GroupID >> s.ListNumber;
				s.Exam[0] = static_cast<TMark>(Get<size_t>(in));
				s.Exam[1] = static_cast<TMark>(Get<size_t>(in));
				s.Exam[2] = static_cast<TMark>(Get<size_t>(in));
				s.Credit[0] = static_cast<TMark>(Get<size_t>(in));
				s.Credit[1] = static_cast<TMark>(Get<size_t>(in));
				s.Credit[2] = static_cast<TMark>(Get<size_t>(in));
				s.Credit[3] = static_cast<TMark>(Get<size_t>(in));
				s.Credit[4] = static_cast<TMark>(Get<size_t>(in));
				in >> s.EditDate >> s.EditTime;
				students.push_back(make_shared<Student>(s));
			}
		}
	}

	~Database() {
		ofstream out(base);
		if (out) {
			out << lastID << ' ' << recordCount << '\n';
			for (size_t i = 0; i < FIELD.size(); ++i) {
				out << setws[i] << ' ';
			}
			out << '\n';
			for (size_t i = 0; i < recordCount; ++i) {
				out << students[i]->ID								<< ' ' 
					<< students[i]->LastName						<< ' ' 
					<< students[i]->FirstName						<< ' ' 
					<< students[i]->SecondName						<< ' '
					<< static_cast<size_t>(students[i]->Sex)		<< ' '
					<< static_cast<size_t>(students[i]->Form)		<< ' '
					<< students[i]->GroupID							<< ' '
					<< students[i]->ListNumber						<< ' '
					<< static_cast<size_t>(students[i]->Exam[0])	<< ' '
					<< static_cast<size_t>(students[i]->Exam[1])	<< ' '
					<< static_cast<size_t>(students[i]->Exam[2])	<< ' '
					<< static_cast<size_t>(students[i]->Credit[0])	<< ' '
					<< static_cast<size_t>(students[i]->Credit[1])	<< ' '
					<< static_cast<size_t>(students[i]->Credit[2])	<< ' '
					<< static_cast<size_t>(students[i]->Credit[3])	<< ' '
					<< static_cast<size_t>(students[i]->Credit[4])	<< ' '
					<< students[i]->EditDate						<< ' '
					<< students[i]->EditTime						<< '\n';
			}
		}
	}

	void Add() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		Caption(x + 1, y, "Создание НОВОЙ записи:");
		auto pStudent = make_shared<Student>();
		for (size_t i = 0U; i < FIELD.size(); ++i) {
			Caption(x + 2, y + 2, "Шаг " + to_string(i) + " из " + to_string(FIELD.size() - 2U) + ":");
			pStudent->Change(static_cast<Field>(i), x + 2, y + 4);
			ERASER.ClearLast();
		}
		ERASER.ClearLast();
		pStudent->ID = ++lastID;
		students.push_back(pStudent);
		RecalcListNums(pStudent->GroupID);
		RecalcSetws(pStudent);
		++recordCount;
	}

	void Edit() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		Caption(x + 1, y, "Редактировать запись:");
		auto pStudent = SelectStudent(x + 2, y + 2);
		Field field = Select<Field>(x + 2, y + 2);
		pStudent->Change(field, x + 2, y + 2);
		RecalcListNums(pStudent->GroupID);
		RecalcSetws(pStudent);
		ERASER.Clear();
	}

	void Show() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		Caption(x + 1, y, "Все студенты:");
		Print(students, x, y + 1);
	}

	void ShowGroup() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		constexpr const char* caption = "Список студентов группы ";
		Caption(x + 1, y, caption);
		string group = SelectGroupID(x + 2, y + 2);
		Caption(x + 1 + length(caption), y, group);

		vector<shared_ptr<Student>> filtered;
		copy_if(students.begin(), students.end(), back_inserter(filtered),
			[&group](shared_ptr<Student>& s) { return s->GroupID == group; });
		Print(filtered, x, y + 1);
	}

	void Top() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		auto top = students;
		sort(top.begin(), top.end(),
			[](const shared_ptr<Student>& lhs, const shared_ptr<Student>& rhs) {
				return lhs->Ratio() < rhs->Ratio();
			});
		Caption(x + 1, y, "Топ студентов:");
		Print(top, x, y + 1);
	}

	void CountBySex() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		vector<shared_ptr<Student>> males;
		vector<shared_ptr<Student>> females;
		for (const auto& pStudent : students) {
			if (pStudent->Sex == TSex::male) males.push_back(pStudent);
			else if (pStudent->Sex == TSex::female) females.push_back(pStudent);
		}
		Caption(x + 1, y, "Мужчины:");
		Print(males, x, y + 1);
		Caption(x + 1, y + males.size() + 4, "Женщины:");
		Print(females, x, y + males.size() + 5);
	}

	void Scholarship() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		vector<shared_ptr<Student>> selection;
		copy_if(students.begin(), students.end(), back_inserter(selection),
			[](const shared_ptr<Student>& student) {
				bool valid = true;
				for (const auto& e : student->Exam)
					valid = valid && (e != TMark::m2 && e != TMark::m3);
				for (const auto& e : student->Credit)
					valid = valid && (e != TMark::m2 && e != TMark::m3);
				return valid && student->Form == TForm::FullTime;
			});
		Caption(x + 1, y, "Список студентов, которые получат стипендию:");
		Print(selection, x, y + 1);
	}

	void NoScholarship() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		vector<shared_ptr<Student>> selection;
		copy_if(students.begin(), students.end(), back_inserter(selection),
			[](const shared_ptr<Student>& student) {
				bool valid = true;
				for (const auto& e : student->Exam)
					valid = valid && (e != TMark::m2 && e != TMark::m3);
				for (const auto& e : student->Credit)
					valid = valid && (e != TMark::m2 && e != TMark::m3);
				return !(valid && student->Form == TForm::FullTime);
			});
		Caption(x + 1, y, "Список студентов, которые НЕ получат стипендию:");
		Print(selection, x, y + 1);
	}

	void Only4() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		vector<shared_ptr<Student>> selection;
		copy_if(students.begin(), students.end(), back_inserter(selection),
			[](const shared_ptr<Student>& student) {
				bool valid = true;
				for (const auto& e : student->Exam)
					valid = valid && (e == TMark::m4);
				for (const auto& e : student->Credit)
					valid = valid && (e == TMark::m4);
				return valid;
			});
		Caption(x + 1, y, "Список студентов, которые учатся только на 4:");
		Print(selection, x, y + 1);
	}

	void Only5() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		vector<shared_ptr<Student>> selection;
		copy_if(students.begin(), students.end(), back_inserter(selection),
			[](const shared_ptr<Student>& student) {
				bool valid = true;
				for (const auto& e : student->Exam)
					valid = valid && (e == TMark::m5);
				for (const auto& e : student->Credit)
					valid = valid && (e == TMark::m5);
				return valid;
			});
		Caption(x + 1, y, "Список студентов, которые учатся только на 5:");
		Print(selection, x, y + 1);
	}

	void Only45() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		vector<shared_ptr<Student>> selection;
		copy_if(students.begin(), students.end(), back_inserter(selection),
			[](const shared_ptr<Student>& student) {
				bool valid = true;
				for (const auto& e : student->Exam)
					valid = valid && (e == TMark::m4 || e == TMark::m5);
				for (const auto& e : student->Credit)
					valid = valid && (e == TMark::m4 || e == TMark::m5);
				return valid;
			});
		Caption(x + 1, y, "Список студентов, которые учатся только на 4 и 5:");
		Print(selection, x, y + 1);
	}

	void ByEditDate() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		constexpr const char* caption = "Добавлены в базу, либо изменены: ";
		Caption(x + 1, y, caption);
		string date = SelectDate(x + 2, y + 2);
		Caption(x + 1 + length(caption), y, date);
		vector<shared_ptr<Student>> am, pm;
		for (const auto& pStudent : students) {
			if (pStudent->EditDate == date)
				switch (pStudent->EditTime) {
				case DayPeriod::AM:
					am.push_back(pStudent);
					break;
				case DayPeriod::PM:
					pm.push_back(pStudent);
					break;
				default: break;
				}
		}
		Caption(x + 1, y + 2, "До полудня:");
		Print(am, x, y + 3);
		Caption(x + 1, y + 6 + am.size(), "После полудня:");
		Print(pm, x, y + 7 + am.size());
	}

	void ByListNumber() {
		ERASER.Clear();
		constexpr int x = 1;
		constexpr int y = 1;
		constexpr const char* caption = "Имеющие номер в списке: ";
		Caption(x + 1, y, caption);
		size_t num = SelectListNum(x + 2, y + 2);
		Caption(x + 1 + length(caption), y, to_string(num));
		vector<shared_ptr<Student>> selection;
		copy_if(students.begin(), students.end(), back_inserter(selection),
			[&num](const shared_ptr<Student>& student) {
				return num == student->ListNumber;
			});
		Print(selection, x, y + 1);
	}

	void FileOutput() {
		constexpr const char* name = "students.txt";
		ofstream out(name);
		if (out) {
			out << left;
			for (size_t i = 0; i < FIELD.size(); ++i)
				out << setw(setws[i]) << *(FIELD.begin() + i);
			out << '\n';
			for (const auto& pStudent : students) {
				for (size_t j = 0; j < FIELD.size(); ++j)
					out << setw(setws[j]) << pStudent->GetField(j);
				out << '\n';
			}
		}
	}

private:
	string base;
	size_t recordCount;
	size_t lastID;
	vector<streamsize> setws;
	vector<shared_ptr<Student>> students;

	//Пересчитывает порядковые номера в группе
	void RecalcListNums(const string& ID) {
		set<shared_ptr<Student>> temp;
		for (const auto& ptr : students)
			if (ptr->GroupID == ID) temp.insert(ptr);
		size_t i = 1;
		for (const auto& ptr : temp)
			ptr->ListNumber = i++;
	}

	// Пересчитывает ширину столбца
	void RecalcSetws(shared_ptr<Student>& s) {
		for (const auto& pStudent : students)
			for (size_t i = 0; i < FIELD.size(); ++i) {
				size_t width = pStudent->GetField(i).size();
				if (width >= setws[i]) setws[i] = width + 1U;
			}
	}

	// Выбрать номер группы в списке
	string SelectGroupID(const int& x, const int& y) {
		set<string> groups;
		for (const auto& pStudent : students)
			groups.insert(pStudent->GroupID);
		Caption(x, y, " Выберите группу из списка: ", Style::Underline);
		vector<string> v(groups.begin(), groups.end());
		size_t i = BasicSelect(v, x + 1, y + 1);
		ERASER.ClearLast();
		return v[i];
	}

	// Выбрать номер в списке
	size_t SelectListNum(const int& x, const int& y) {
		size_t maxNum = 0U;
		for (const auto& pStudent : students)
			if (pStudent->ListNumber > maxNum) maxNum = pStudent->ListNumber;
		vector<string> listNums(maxNum);
		for (size_t i = 0U; i < listNums.size(); ++i)
			listNums[i] = to_string(i + 1U);
		Caption(x, y, " Выберите из списка порядковый номер студента: ", Style::Underline);
		size_t i = BasicSelect(listNums, x + 1, y + 1);
		ERASER.ClearLast();
		return i + 1U;
	}

	// Выбрать дату
	string SelectDate(const int& x, const int& y) {
		set<string> dates;
		for (const auto& pStudent : students)
			dates.insert(pStudent->EditDate);
		Caption(x, y, " Выберите дату редактирования из списка: ", Style::Underline);
		vector<string> v(dates.begin(), dates.end());
		size_t i = BasicSelect(v, x + 1, y + 1);
		ERASER.ClearLast();
		return v[i];
	}

	// Выбрать студента
	shared_ptr<Student> SelectStudent(const int& x, const int& y) {
		vector<string> names(students.size());
		for (size_t i = 0; i < students.size(); ++i)
			names[i] = students[i]->FullName();
		Caption(x, y, " Выберите студента из списка: ", Style::Underline);
		size_t num = BasicSelect(names, x + 1, y + 1);
		ERASER.ClearLast();
		return students[num];
	}

	// Рисует таблицу
	void Print(const vector<shared_ptr<Student>>& pStudents, const int& x, const int& y) {
		{	// Шапка
			ostringstream iss(" ");
			iss << left;
			for (size_t i = 0; i < FIELD.size(); ++i)
				iss << setw(setws[i]) << *(FIELD.begin() + i);
			string line = iss.str();
			Caption(x, y + 1, line, Style::Underline);

			// ВСЕГО ЗАПИСЕЙ
			string records = "ВСЕГО ЗАПИСЕЙ: " + to_string(pStudents.size());
			Caption(x + line.size() - records.size(), y, records);
		}

		// Тело
		for (size_t i = 0; i < pStudents.size(); ++i) {
			ostringstream iss(" ");
			iss << left;
			for (size_t j = 0; j < FIELD.size(); ++j)
				iss << setw(setws[j]) << pStudents[i]->GetField(j);
			Caption(x, y + i + 2, iss.str());
		}
	}
};

// ================================================================================================
// TCommand
// Фактически главное меню

auto COMMAND = {
	"Добавить новую запись",
	"Изменить существующую запись",
	"Список всех студентов",
	"Список студентов группы",
	"Топ студентов",
	"Мужчины и женщины",
	"Получат стипендию",
	"Не получат стипендию",
	"Учатся только на 4",
	"Учатся только на 5",
	"Учатся только на 4 и 5",
	"Записи по дате",
	"Записи по номеру в списке",
	"Вывод в файл",
	"Выход",
};

enum class TCommand : size_t {
	Add,
	Edit,
	Show,
	ShowGroup,
	Top,
	CountBySex,
	Scholarship,
	NoScholarship,
	Only4,
	Only5,
	Only45,
	ByEditDate,
	ByListNumber,
	FileOutput,
	Exit
};

template<>
TCommand Select(const int& x, const int& y) {
	Database db;
	size_t selected = 0U;
	while (true) {
		Caption(x, y, " Выберите КОМАНДУ:           ", Style::Underline);
		selected = BasicSelect(COMMAND, x + 1, y + 1, selected);
		auto command = static_cast<TCommand>(selected);
		switch (command) {
		case TCommand::Add:				db.Add();			break;
		case TCommand::Edit:			db.Edit();			break;
		case TCommand::Show:			db.Show();			break;
		case TCommand::ShowGroup:		db.ShowGroup();		break;
		case TCommand::Top:				db.Top();			break;
		case TCommand::CountBySex:		db.CountBySex();	break;
		case TCommand::Scholarship:		db.Scholarship();	break;
		case TCommand::NoScholarship:	db.NoScholarship();	break;
		case TCommand::Only4:			db.Only4();			break;
		case TCommand::Only5:			db.Only5();			break;
		case TCommand::Only45:			db.Only45();		break;
		case TCommand::ByEditDate:		db.ByEditDate();	break;
		case TCommand::ByListNumber:	db.ByListNumber();	break;
		case TCommand::FileOutput:		db.FileOutput();	break;

		case TCommand::Exit:
			ERASER.Clear();
			return TCommand::Exit;
			break;
		default:
			break;
		}
	}
}

// ================================================================================================
// Задание 2

// Статус участника конференции
enum class TStatus {
	Speaker,	// Выступающий
	Listener	// Слушатель
};

ostream& operator<<(ostream& os, const TStatus& status) {
	switch (status) {
	case TStatus::Speaker:	return os << "Выступающий";
	case TStatus::Listener:	return os << "Слушатель";
	default:				return os;
	}
}

// Ученая степень участника конференции
enum class TDegree {
	Abiturient,	// Абитуриент
	Student,	// Студент
	Bachelor,	// Бакалавр
	Master,		// Магистр
	Aspirant,	// Аспирант
	Candidate,	// Кандидат наук
	Doctor,		// Доктор наук
	Docent,		// Доцент (звание)
	Professor,	// Профессор (звание)
};

ostream& operator<<(ostream& os, const TDegree& degree) {
	switch (degree) {
	case TDegree::Abiturient:	return os << "Абитуриент";
	case TDegree::Student:		return os << "Студент";
	case TDegree::Bachelor:		return os << "Бакалавр";
	case TDegree::Master:		return os << "Магистр";
	case TDegree::Aspirant:		return os << "Аспирант";
	case TDegree::Candidate:	return os << "Кандидат наук";
	case TDegree::Doctor:		return os << "Доктор наук";
	case TDegree::Docent:		return os << "Доцент";
	case TDegree::Professor:	return os << "Профессор";
	default:					return os;
	}
}

// Участник конференции
struct Visitor {
	explicit Visitor(
		const string& FirstName,
		const string& SecondName,
		const string& LastName,
		const TStatus& Status,
		const TDegree& Degree,
		const string& University,
		const string& Theme
	)
		: FirstName(FirstName)
		, SecondName(SecondName)
		, LastName(LastName)
		, Status(Status)
		, Degree(Degree)
		, University(University)
		, Theme(Theme)
	{
	}

	string	FirstName;	// Имя
	string	SecondName;	// Отчество
	string	LastName;	// Фамилия

	TStatus	Status;		// Статус
	TDegree	Degree;		// Ученая степень

	string	University;	// Название университета
	string	Theme;		// Тема выступления
};

ostream& operator<<(ostream& os, const Visitor& visitor) {
	return os << "ФИО: " << visitor.LastName << ' ' << visitor.FirstName << ' ' << visitor.SecondName << '\n'
		<< "Ученая степень: " << visitor.Degree << "\nУЗ: " << visitor.University << '\n'
		<< "Статус: " << visitor.Status
		<< (visitor.Status == TStatus::Speaker ? " Тема: " + visitor.Theme : "") << '\n';
}

template<class Container, class Predicate>
void Print(const string& text, const Container& container, const Predicate& predicate) {
	ofstream out("task2.txt", ios::app);
	if (out) {
		out << "====================== " << text << " =====================================================\n\n";
		copy_if(container.begin(), container.end(), ostream_iterator<Visitor>(out, "\n"), predicate);
	}
}

void Task2() {
	constexpr auto task2 = "Задание 2";
	constexpr auto result = "Результат работы выведен в файл task2.txt в каталоге программы";
	{ofstream out("task2.txt"); }
	Caption(CONSOLE.CX() - length(task2) / 2, CONSOLE.CY(), task2, Style::Underline);
	Sleep(2000);
	initializer_list<Visitor> list = {
		Visitor("Чуканова",		"Кира",			"Федотовна",		TStatus::Listener,	TDegree::Professor,	"УрФУ",	""),
		Visitor("Этуша",		"Марианна",		"Елизаровна",		TStatus::Listener,	TDegree::Student,	"ННГУ",	""),
		Visitor("Серебров",		"Онуфрий",		"Остапович",		TStatus::Speaker,	TDegree::Student,	"ЛЭТИ",	"Медианы биномиальных и пуассоновских распределений"),
		Visitor("Квакина",		"Елена",		"Брониславовна",	TStatus::Speaker,	TDegree::Candidate,	"МФТИ",	"Вопросы рациональности куммеровых поверхностей над конечными полями и их приложения в криптографии"),
		Visitor("Нюхалов",		"Викентий",		"Прокофиевич",		TStatus::Listener,	TDegree::Student,	"ЛЭТИ",	""),
		Visitor("Муленко",		"Валентина",	"Фомевна",			TStatus::Listener,	TDegree::Student,	"УрФУ",	""),
		Visitor("Шибалова",		"Милена",		"Афанасиевна",		TStatus::Speaker,	TDegree::Professor,	"ННГУ",	"Дискретные группы отражений в пространствах Лобачевского"),
		Visitor("Крылаев",		"Лука",			"Давыдович",		TStatus::Listener,	TDegree::Student,	"ЛЭТИ",	""),
		Visitor("Калмыкова",	"Роза",			"Тихоновна",		TStatus::Listener,	TDegree::Student,	"ЛЭТИ",	""),
		Visitor("Зубока",		"Элеонора",		"Семеновна",		TStatus::Speaker,	TDegree::Aspirant,	"ЛЭТИ",	"Обратные задачи для параметров графов"),
		Visitor("Журавлев",		"Матвей",		"Аполлинариевич",	TStatus::Listener,	TDegree::Student,	"МФТИ",	""),
		Visitor("Зуев",			"Савелий",		"Никанорович",		TStatus::Listener,	TDegree::Student,	"ННГУ",	""),
		Visitor("Антонова",		"Ефросиния",	"Марковна",			TStatus::Listener,	TDegree::Candidate,	"ЛЭТИ",	""),
		Visitor("Монакова",		"Татьяна",		"Данилевна",		TStatus::Speaker,	TDegree::Student,	"ННГУ",	""),
		Visitor("Гречко",		"Влада",		"Марковна",			TStatus::Speaker,	TDegree::Student,	"ЛЭТИ",	"Системы алгебраических уравнений общего положения с единственным решением  "),
		Visitor("Сарнычева",	"Маргарита",	"Олеговна",			TStatus::Listener,	TDegree::Professor,	"МФТИ",	""),
		Visitor("Кэбина",		"Ярослава",		"Родионовна",		TStatus::Listener,	TDegree::Student,	"МФТИ",	""),
		Visitor("Веселовский",	"Моисей",		"Назарович",		TStatus::Speaker,	TDegree::Professor,	"ЛЭТИ",	"Алгебраическая структура двойных чисел Гурвица старших родов"),
		Visitor("Мышкина",		"Кристина",		"Трофимовна",		TStatus::Listener,	TDegree::Student,	"ЛЭТИ",	""),
		Visitor("Лассмана",		"Роза",			"Захаровна",		TStatus::Listener,	TDegree::Student,	"УрФУ",	""),
	};
	
	// Вывести всех
	Print("Все студенты", list, [](const Visitor& visitor) { return true; });

	// Вывести всех иногородних приглашенных
	Print("Все иногородние приглашенные", list,
		[](const Visitor& visitor) { return visitor.University != "ЛЭТИ"; });

	// Вывести всех студентов, кто приглашен выступать
	Print("Все студенты, кто приглашен выступать", list, [](const Visitor& visitor) {
		return visitor.Degree == TDegree::Student && visitor.Status == TStatus::Speaker;
		});

	// Вывести всех студентов университета, в котором проходит конференция
	Print("Все студенты университета, в котором проходит конференция",
		list, [](const Visitor& visitor) {
			return visitor.Degree == TDegree::Student && visitor.University == "ЛЭТИ";
		});

	Caption(CONSOLE.CX() - length(result) / 2, CONSOLE.CY() + 1, result, Style::Default);
	Sleep(4000);
	ERASER.Clear();
}

// ================================================================================================
// main

int main() {
	setlocale(0, "");
	CONSOLE.FullScreen();
	CONSOLE.ActivateWindowsTerminal();
	cout << HIDE;

	// Основное задание
	Select<TCommand>(CONSOLE.MX() - 35, 3);

	// Задание по вариантам
	Task2();
}
