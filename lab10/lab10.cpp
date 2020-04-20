#include <iostream>
#include <conio.h>
#include <Windows.h>
using namespace std;

std::string operator"" _s(const char* str, size_t size)
{
	char buf[128];
	CharToOemA(str, buf);
	return std::string(buf, size);
}

string f(const char* str) {
	char b[80];
	OemToCharA(str, b);
	return b;
}


int main()
{
	setlocale(0, "C");
	/*cout << "GetConsoleCP; " << GetConsoleCP()
		<< "\nGetConsoleOutputCP: " << GetConsoleOutputCP() << '\n\n';*/

		/*cout << "setlocale(0, \"C\"):\n";
		for (int i = -80; i <= -32; ++i) {
			cout << i << ' ' << char(i) << ' ';
		}*/

	char b[] = "задницаёё\n";
	char c[15];
	CharToOemA(b, c);
	cout << "CharToOEm: " << c; //работает

	//const char* c = "задницаёё\n";
	//char a[];


	TCHAR a[] = TEXT("задницаёё\n");
	int n = 0;
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), a, wcslen(a), (LPDWORD)&n, nullptr); //работает

	wcout << L"wcout задницаёё\n"; // только en
	cout << "\n=============\n";
	cout << "задницаёё\n"_s;

	return 0;
	string s;
	for (int ch; ;)
		if (_kbhit()) {
			ch = _getch();
			switch (ch)
			{
			case 27: return 0;
			case 13:
				setlocale(0, "C");
				cout << "setlocale(0, \"C\"):\n";
				for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
					cout << i << ' ' << char(i) << ' ';
				}
				cout << '\n';
				setlocale(0, "");
				cout << "setlocale(0, \"\"):\n";
				for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
					cout << i << ' ' << char(i) << ' ';
				}
				cout << '\n';
			default:
				cout << "num: " << ch << "\n";
				setlocale(0, "C");
				cout << "setlocale(0, \"C\") " << char(ch) << "  out:";
				s = char(ch);
				cout << s << "\n";
				setlocale(0, "");
				cout << "setlocale(0, \"\") " << char(ch) << "  out:";
				s = char(ch);
				cout << s << "\n\n";
			}
		}
}
