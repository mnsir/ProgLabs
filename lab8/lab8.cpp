#include <iostream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <limits.h>
#include <conio.h>

//using namespace std;


constexpr size_t		OUTPUT_MAX = 99;
constexpr int			GEN_MAX = 99;
constexpr const char	INPUT_FILE[] = "input1.txt";
// ================================================================================================
#include <windows.h>
//Активирует режим терминала для Windows
void ActivateWindowsTerminal() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;// GetLastError();
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) return;// GetLastError();
	if (!SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING));// return GetLastError();
}
//
auto FullScreen() {
	COORD coord;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	coord.X = GetSystemMetrics(SM_CXMIN);
	coord.Y = GetSystemMetrics(SM_CYMIN);
	SetConsoleScreenBufferSize(handle, coord);
	SetConsoleDisplayMode(handle, CONSOLE_FULLSCREEN_MODE, &coord);
	return std::make_pair(coord.X, coord.Y);
}
// ================================================================================================
// VT100
#define SAVE			"\033[s"			//Сохраняет положение курсора
#define LOAD			"\033[u"			//Восстанавливает положение курсора
#define SET(row, col)	"\033[" << row << ';' << col << 'H'	//Задает положение курсора (строка, столбец)
#define ERASE(n)		"\033[" << n << 'J'	//Очищает окно терминала: 0 - от курсора, 1 - до курсора, 2 - всё окно
#define ERASE_LINE(n)	"\033[" << n << 'K'	//Очищает строку терминала: 0 - от курсора, 1 - до курсора, 2 - всю строку

// ================================================================================================
// strings
#define CASE_1 "\n\n  Введите арабскими цифрами целое положительное число N, обозначающее количество\n"   \
"  элементов в массиве, который будет автоматически заполняться случайными числами от 0 до 99\n  N = "
#define CASE_2 "\n\n  Вводите поочередно элементы массива типа int. Размер определяется автоматически\n"	\
"  по количеству введенных элементов. Завершение ввода стандартно для консолей систем Windows (^Z)\n  NUM = "
#define CASE_3 "\n\n  Массив считывается с файла input.txt, который находится в директории программы."
#define CASE_4 "\n\n  Введите существующий индекс элемента, на место которого следует вставить новый.\n"
#define CASE_5 "\n\n  Введите существующий индекс элемента, который следует удалить.\n  INDEX = "
#define CASE_6 "\n\n  Введите существующее значение контейнера, которое следует удалить.\n  DATA = "
#define CASE_7 "\n\n  Введите существующий индекс элемента, элемент по которому следует вывести\n  INDEX = "
#define CASE_8 "\n\n  Введите существующее значение контейнера, индекс которого следует вывести\n  DATA = "
#define CASE_9 "\n\n  Введите арабскими цифрами целое положительное число N, обозначающее количество чисел, которые нужно рандомно вставить в контейнер\n  N = "

constexpr const char CAPTION_MSG[] = "Лабораторная работа №2.";
constexpr const char WELCOME_MSG[] = "Нажмите на клавиатуре клавишу, соответствующую команде:";
constexpr const char* MENU_ITEM[] = {
"  1.Сгенерировать автоматически",
"  2.Ввести вручную",
"  3.Считать из файла",
"  4.Вставить элемент",
"  5.Удалить элемент по индексу",
"  6.Удалить элемент по значению",
"  7.Получить значение по индексу",
"  8.Получить индекс по значению",
"  9.Рандомно вставить рандомные значения",
"ESC.Завершение работы программы",
};

// ================================================================================================
// profiler
#include<chrono>

class LogDuration {
public:
	explicit LogDuration(const std::string& msg = "")
		: message(msg + ": ")
		, start(std::chrono::steady_clock::now())
	{
	}

	~LogDuration() {
		auto finish = std::chrono::steady_clock::now();
		auto dur = finish - start;
		std::cout << message
			<< std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()
			<< " ms" << std::endl;
	}
private:
	std::string message;
	std::chrono::steady_clock::time_point start;
};
#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message)			\
LogDuration UNIQ_ID(__LINE__){message};

// ================================================================================================
// рандом
#include<random>
template<typename T>
T rnd(const T& border) {
	const int64_t t = std::chrono::system_clock::now().time_since_epoch().count();
	std::minstd_rand gen(static_cast<uint32_t>(t));
	return gen() % border;
}
// ================================================================================================


bool isrus(const int& i) {
	//принимает int из _getch() библиотеки conio.h. Правильно преобразуется в unsigned char.
	//setlocale не использовать!!!!
	// (А - Я) => 128 - 159
	// (а - п) => 160 - 175
	// (р - я) => 224 - 239
	// Ё - 240   ё - 241
	return i >= 128 && i <= 175 || i >= 224 && i <= 239;
}

template<typename T>
T Get(std::istream& is) {
	is.clear();
	T x;
	is >> x;
	return x;
}

// ================================================================================================
// Динамический массив
class MyArray {
public:
	explicit MyArray() : begin(nullptr), end(nullptr), size(0U), max_size(0U)
	{
	}

	~MyArray() { Destroy(); }

	bool Empty() const { return !size; }

	const size_t& Size() const { return size; }

	void PushBack(const int& data) {
		size_t i = size;
		Resize(size + 1U);
		begin[i] = data;
	}

	void PopBack() {
		--size;
		--end;
	}

	int* Begin() const { return begin; }

	int* End() const { return end; }

	// Мощное изменение размера
	void Resize(const size_t& new_size) {

		// Если новый размер равен старому - выходим
		if (size == new_size) return;

		// Новый размер равен 0 - стираем
		if (new_size == 0U) Destroy();

		// Если массив не создан - создаем
		else if (begin == nullptr) Create(new_size);

		// Новый размер меньше масимального - меняем значение размера
		else if (new_size <= max_size) {
			size = new_size;
			end = begin + size;
		}

		// Если больше - сздаем новый, подходящего размера и копируем содержимое старого
		else if (new_size > max_size) {
			int* oldBegin = begin;
			int* oldEnd = end;
			Create(new_size);
			std::copy(oldBegin, oldEnd, begin);
			delete[] oldBegin;
		}
	}

	void Erase() {
		end = begin;
		size = 0U;
	}

	void Erase(const size_t& pos) {
		if (pos < size) {
			for (size_t i = pos + 1; i < size; ++i)
				std::swap(begin[i], begin[i - 1]);
			PopBack();
		}
	}

	void Insert(const size_t& pos, const int& data) {
		if (pos <= size) {
			PushBack(data);
			if (size > 1) {
				for (size_t i = size - 1; i > pos; --i) {
					std::swap(begin[i], begin[i - 1]);
				}
			}
		}
		else std::cout << "индекс выходит за пределы массива!" << std::endl;
	}

	void FillByFile() {
		Resize(0U);
		std::ifstream input("input1.txt");
		for (int n = 0; input >> n; ) {
			PushBack(n);
		}
	}

	int& operator[](const size_t& i) { return *(begin + i); }
	const int& operator[](const size_t& i) const { return *(begin + i); }

private:
	size_t calc_max(const size_t& s) {
		size_t max = 32U;
		while (max < s) {
			max <<= 1U;
		}
		return max;
	}

	void Create(const size_t& new_size) {
		max_size = calc_max(new_size);
		begin = new int[max_size];
		end = begin + new_size;
		size = new_size;
	}

	void Destroy() {
		delete[] begin;
		begin = nullptr;
		end = nullptr;
		max_size = 0U;
		size = 0U;
	}

	int* begin;
	int* end;
	size_t size;
	size_t max_size;
};

std::ostream& operator<<(std::ostream& os, const MyArray& arr) {
	os << "array: ";
	if (arr.Empty()) os << "пустой!";
	else if (arr.Size() > OUTPUT_MAX) os << "слишком много элементов!";
	else {
		os << '{';
		std::copy(arr.Begin(), arr.End(), std::ostream_iterator<int>(std::cout, ", "));
		os << "\b\b}";
	}
	return os << " size: " << arr.Size();
}

void FillAuto(MyArray& arr, const size_t& n) {
	arr.Resize(n);
	for (size_t i = 0; i < n; ++i) {
		arr[i] = rnd<int>(GEN_MAX);
	}
}

// ================================================================================================
// Двусвязный список
class MyList {
public:
	explicit MyList() : head(nullptr), tail(nullptr), size(0U)
	{
	}

	bool Empty() const { return head == nullptr; }

	size_t Size() const { return size; }

	void PopFront() {
		if (head) {
			Node* node = head;
			head = head->next;
			if (head) head->prev = nullptr;
			delete node;
			--size;
		}
	}

	void PopBack() {
		if (tail) {
			Node* node = tail;
			tail = tail->prev;
			if (tail) tail->next = nullptr;
			delete node;
			--size;
		}
	}

	void Erase() {
		Node* node;
		while (head != nullptr) {
			node = head;
			head = head->next;
			delete node;
		}
		tail = nullptr;
		size = 0U;
	}

	void Erase(const size_t& index) {
		if (size && size != ~0U) {
			if (index == 0U) PopFront();
			else if (index == size - 1U) PopBack();
			else if (index < size) {
				Node* node = head;
				for (size_t i = 0U; i < index; ++i) {
					node = node->next;
				}
				node->prev->next = node->next;
				node->next->prev = node->prev;
				delete node;
				--size;
			}
		}
	}

	void PushFront(const int& Data) {
		Node* node = new Node();
		node->data = Data;
		if (head == nullptr) {
			head = node;
			tail = node;
		}
		else {
			head->prev = node;
			node->next = head;
			head = node;
		}
		++size;
	}

	void PushBack(const int& Data) {
		Node* node = new Node();
		node->data = Data;
		if (head == nullptr) {
			head = node;
			tail = node;
		}
		else {
			tail->next = node;
			node->prev = tail;
			tail = node;
		}
		++size;
	}

	void Insert(const size_t& pos, const int& data) {
		if (size == 0 || pos == 0) PushFront(data);
		else if (pos == size) PushBack(data);
		else if (pos > size) std::cout << "ААААА!! индекс выходит за пределы списка!";
		else {
			Node* node = new Node();
			node->data = data;
			Node* current = head;
			for (size_t i = 0U; i < pos; ++i) {
				current = current->next;
			}
			current->prev->next = node;
			node->next = current;
			node->prev = current->prev;
			current->prev = node;
			++size;
		}
	}

	const int& operator[](const size_t& index) const {
		Node* node = head;
		for (size_t i = 0; i < index; ++i) {
			node = node->next;
		}
		return node->data;
	}

	size_t IndexOf(const int& data) const {
		Node* node = head;
		if (node == nullptr) return ~0U;
		size_t index = 0;
		while (node != nullptr) {
			if (node->data == data) break;
			node = node->next;
			++index;
		}
		return index;
	}

	friend std::ostream& operator<<(std::ostream&, const MyList&);

private:
	struct Node {
		int data = 0;
		Node* prev = nullptr;
		Node* next = nullptr;
	};
	Node* head;
	Node* tail;
	size_t size;
};

// Перегрузка оператора вывода
std::ostream& operator<<(std::ostream& os, const MyList& list) {
	os << "list: ";
	if (list.head == nullptr) os << "пустой!";
	else if (list.Size() > OUTPUT_MAX) os << "слишком много элементов!";
	else {
		os << '{';
		MyList::Node* node = list.head;
		while (node != nullptr) {
			os << node->data << ", ";
			node = node->next;
		}
		os << "\b\b}";
	}
	return os << " size: " << list.Size();
}

// Заполняет рандомно count элементов от 0 до 99
void FillAuto(MyList& list, const size_t& n) {
	if (!list.Empty()) list.Erase();
	for (size_t i = 0; i < n; ++i) {
		list.PushBack(rnd<int>(GEN_MAX));
	}
}

// Заполняет значениями из файла
template<class Container>
void FillByFile(Container& container) {
	if (!container.Empty()) container.Erase();
	int temp;
	for (std::ifstream file(INPUT_FILE); file >> temp; ) {
		container.PushBack(temp);
	}
}

using namespace std;

int main() {
	setlocale(0, "");
	ActivateWindowsTerminal();
	auto width = FullScreen().first;
	int height = 3;
	std::cout << SET(height - 1, (width - strlen(CAPTION_MSG)) / 2) << CAPTION_MSG
		<< SET(++height, (width - strlen(WELCOME_MSG)) / 2) << WELCOME_MSG;
	++height;
	size_t maxLenght = 0;
	for (size_t i = 0; i < 5; ++i) {
		size_t n = strlen(MENU_ITEM[i]);
		if (n > maxLenght) maxLenght = n;
	}
	size_t col1 = width / 2 - maxLenght - 5;
	size_t col2 = width / 2 + 5;
	for (size_t i = 0; i < 5; ++i) {
		std::cout << SET(++height, col1) << MENU_ITEM[i] << SET(height, col2) << MENU_ITEM[i + 5];
	}
	height += 2;
	int hArr = height + 10;
	int hList = height + 20;
	MyArray arr;
	MyList list;

	bool repeat = true;
	for (int n; repeat; ) {
		if (_kbhit()) {
			n = _getch();
			std::cout << SET(height, 3) << ERASE(0);
			switch (n) {
			case 27:
				repeat = false;
				break;

				// 1 - Создать автоматически
			case 49: {
				std::cout << MENU_ITEM[0] << CASE_1;
				size_t n = Get<size_t>(cin);
				{
					LOG_DURATION("массив")
						FillAuto(arr, n);
				}
				{
					LOG_DURATION("список")
						FillAuto(list, n);
				}
				break;
			}

				   // 2 - Задать значения вручную
			case 50: {
				arr.Erase();
				list.Erase();
				std::cout << MENU_ITEM[1] << CASE_2 << SAVE;
				{
					LOG_DURATION("вы вводили значения")
						for (int i; cin >> i; std::cout << LOAD << ERASE(0) << SET(hArr, 3) << arr << SET(hList, 3) << list << LOAD) {
							arr.PushBack(i);
							list.PushBack(i);
						}
				}
				cin.clear();
				break;
			}

				   // 3 - Считать из файла
			case 51: {
				std::cout << MENU_ITEM[2] << CASE_3 << endl;
				{
					LOG_DURATION("массив")
						FillByFile(arr);
				}
				{
					LOG_DURATION("список")
						FillByFile(list);
				}
				break;
			}

			// 4 - Вставить значение
			case 52: {
				std::cout << MENU_ITEM[3] << CASE_4 << "  POS = " << SAVE << SET(hArr, 3) << arr << SET(hList, 3) << list << LOAD;
				size_t pos = Get<size_t>(cin);
				std::cout << "  Далее введите его значание:\n  DATA = ";
				int data = Get<int>(cin);
				std::cout << LOAD << ERASE(0);
				{
					LOG_DURATION("массив")
					arr.Insert(pos, data);
				}
				{
					LOG_DURATION("список")
					list.Insert(pos, data);
				}
				break;
			}

				   // 5 - Удалить по индексу
			case 53: {
				std::cout << MENU_ITEM[4] << CASE_5 << SAVE << SET(hArr, 3) << arr << SET(hList, 3) << list << LOAD;
				const auto index = Get<size_t>(cin);
				std::cout << LOAD << ERASE(0);
				{
					LOG_DURATION("массив")
					arr.Erase(index);
				}
				{
					LOG_DURATION("список")
					list.Erase(index);
				}
				break;
			}

			// 6 - Удалить по значению
			case 54: {
				std::cout << MENU_ITEM[5] << CASE_6 << SAVE << SET(hArr, 3) << arr << SET(hList, 3) << list << LOAD;
				const int data = Get<int>(cin);
				std::cout << LOAD << ERASE(0);
				auto index = std::distance(arr.Begin(), std::find(arr.Begin(), arr.End(), data));
				{
					LOG_DURATION("массив")
					arr.Erase(index);
				}
				index = list.IndexOf(data);
				{
					LOG_DURATION("список")
					list.Erase(index);
				}
				break;
			}

				   // 7 - Получить по индексу
			case 55: {
				std::cout << MENU_ITEM[6] << CASE_7 << SAVE << SET(hArr, 3) << arr << SET(hList, 3) << list << LOAD;
				const auto index = Get<size_t>(cin);
				size_t answer1, answer2;
				{
					LOG_DURATION("массив")
					answer1 = arr[index];
				}
				{
					LOG_DURATION("список")
					answer2 = list[index];
				}
				std::cout << "arr[" << index << "] = " << answer1 << "\nlist[" << index << "] = " << answer2 << endl;
				break;
			}

				   // 8 - Получить по значению
			case 56: {
				std::cout << MENU_ITEM[7] << CASE_8 << SAVE << SET(hArr, 3) << arr << SET(hList, 3) << list << LOAD;
				const auto data = Get<int>(cin);
				size_t index1, index2;
				{
					LOG_DURATION("массив")
						index1 = std::distance(arr.Begin(), std::find(arr.Begin(), arr.End(), data));
				}
				{
					LOG_DURATION("список")
						index2 = list.IndexOf(data);
				}
				std::cout << "массив: ";
				if (index1 != arr.Size()) std::cout << "" << index1 << endl;
				else std::cout << "значения нет" << endl;
				std::cout << "список: ";
				if (index2 != list.Size()) std::cout << "" << index2;
				else std::cout << "значения нет";
				break;
			}

				   // 9 - Задание 19
			case 57: {
				std::cout << MENU_ITEM[8] << CASE_9 << SAVE << SET(hArr, 3) << arr << SET(hList, 3) << list << LOAD;
				size_t N = Get<size_t>(cin);
				{
					LOG_DURATION("массив")
						for (size_t i = 0; i < N; ++i) {
							arr.Insert(rnd<size_t>(arr.Size() + 1U), rnd<int>(GEN_MAX));
						}
				}
				{
					LOG_DURATION("список")
						for (size_t i = 0; i < N; ++i) {
							list.Insert(rnd<size_t>(list.Size() + 1U), rnd<int>(GEN_MAX));
						}
				}
				std::cout << LOAD << ERASE(0);
				break;
			}
			default:
				std::cout << "Неизвестный символ!" << endl;
			}
			std::cout << SET(hArr, 3) << arr << SET(hList, 3) << list;
		}
	}
	std::cout << SET(1, 1) << ERASE(0);
	return 0;
}
