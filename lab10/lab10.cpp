#pragma once

#include <iostream>
#include <fstream>
#include <queue>

#include "qwe_getter.h"
#include "qwe_profiler.h"
#include "qwe_random.h"
#include "qwe_simple_menu.h"
#include "qwe_console.h"
#include "qwe_array.h"

Console CONSOLE;

using namespace std;

struct Node {
	int value = 0;
	Node* left = nullptr;
	Node* right = nullptr;
};

class BinaryTree {
public:
	BinaryTree() : root(nullptr) {}

	// Оболочки

	void Insert(const int& num) { _Insert(root, num); }
	void InsertTest(const int& num, const int& task) {
		ofstream out("reply.txt", ios::app);
		if (out) {
			out << "*******************************************************************************************\n\n";
			out << "Ответ к заданию " << task << ".\n";
			out << "Вставка " << num << ".\n";
			_InsertTest(out, root, num);
			out << "Вставка завершена.\n\n";
		}
	}
	void Delete(const int& num) { _Delete(root, num); }
	void DeleteTest(const int& num, const int& task) {
		ofstream out("reply.txt", ios::app);
		if (out) {
			out << "*******************************************************************************************\n\n";
			out << "Ответ к заданию " << task << ".\n";
			out << "Удаление " << num << ".\n";
			_DeleteTest(out, root, num);
			out << "Удаление завершено.\n\n";
		}
	}
	Node* Find(const int& num) { return _Find(root, num); }

	void ForEachForward() const { _ForEachForward(root); }
	void ForEachBackward() const { _ForEachBackward(root); }

	void ForEachWide() const {
		if (nullptr == root) return;
		queue<Node*> nodes;
		nodes.push(root);
		while (!nodes.empty()) {
			Node*& pNode = nodes.front();
			std::cout << '[' << pNode->value << "] ";
			if (pNode->left) nodes.push(pNode->left);
			if (pNode->right) nodes.push(pNode->right);
			nodes.pop();
		}
	}

	void Print() const {
		std::cout << "Вывод дерева:\t\033[41;97m  \033[0m - левое поддерево\t\033[42;97m  \033[0m - правое поддерево\n\n";
		setlocale(0, "C");
		_Print(root);
		setlocale(0, "");
		std::cout << '\n';
	}

	void PrintClean(ostream& os) const { _PrintClean(os, root); }

private:
	// Если элемент существует, то.. ничего не происходит, как в std::set
	void _Insert(Node*& pNode, const int& num) {
		if (pNode == nullptr) {
			pNode = new Node;
			pNode->value = num;
		}
		else if (num < pNode->value) _Insert(pNode->left, num);
		else if (num > pNode->value) _Insert(pNode->right, num);
	}

	void _InsertTest(ostream& os, Node*& pNode, const int& num) {
		if (pNode == nullptr) {
			os << "Эта ячейка пуста. Вставляем " << num << " сюда.\n";
			pNode = new Node;
			pNode->value = num;
		}
		else if (num < pNode->value) {
			os << num << " меньше " << pNode->value << ". Идем в ЛЕВОЕ поддерево";
			if (pNode->left) os << " со значением " << pNode->left->value;
			os << ".\n";
			_InsertTest(os, pNode->left, num);
		}
		else if (num > pNode->value) {
			os << num << " больше " << pNode->value << ". Идем в ПРАВОЕ поддерево";
			if (pNode->right) os << " со значением " << pNode->right->value;
			os << ".\n";
			_InsertTest(os, pNode->right, num);
		}
		else os << "Такое значение уже есть.\n";
	}

	// 
	Node* _Find(Node*& pNode, const int& num) {
		if (!pNode) return nullptr;
		if (pNode->value == num) return pNode;
		
		Node* pSearch = nullptr;
		if (num < pNode->value && pNode->left) pSearch = _Find(pNode->left, num);
		if (pSearch != nullptr) return pSearch;
		if (num > pNode->value && pNode->right) pSearch = _Find(pNode->right, num);
		return pSearch;
	}

	void _Delete(Node*& pNode, const int& num) {
		if (pNode == nullptr) return;
		else if (num < pNode->value) _Delete(pNode->left, num);
		else if (num > pNode->value) _Delete(pNode->right, num);
		else {
			// Нет детей
			if (pNode->right == nullptr && pNode->left == nullptr) {
				delete pNode;
				pNode = nullptr;
			}
			// Один
			else if (pNode->right == nullptr) {
				Node* temp = pNode;
				pNode = pNode->left;
				delete temp;
			}
			else if (pNode->left == nullptr) {
				Node* temp = pNode;
				pNode = pNode->right;
				delete temp;
			}
			// Оба
			else {
				Node* temp = pNode->left;
				while (temp->right != nullptr)
					temp = temp->right;
				pNode->value = temp->value;
				_Delete(pNode->left, temp->value);
			}
		}
	}
	
	void _DeleteTest(ostream& os, Node*& pNode, const int& num) {
		if (pNode == nullptr) {
			os << "Поддерева не существует.\n";
			return;
		}
		else if (num < pNode->value) {
			os << num << " меньше " << pNode->value << ". Ищем " << num
				<< " в ЛЕВОМ поддереве";
			if (pNode->left) os << " со значением " << pNode->left->value;
			os << ".\n";
			_DeleteTest(os, pNode->left, num);
		}
		else if (num > pNode->value) {
			os << num << " больше " << pNode->value << ". Ищем " << num
				<< " в ПРАВОМ поддереве";
			if (pNode->right) os << " со значением " << pNode->right->value;
			os << ".\n";
			_DeleteTest(os, pNode->right, num);
		}
		else {
			os << num << " найден!\n";
			// Нет детей
			if (pNode->right == nullptr && pNode->left == nullptr) {
				os << pNode->value << " является листом. Удаляем лист.\n";
				delete pNode;
				pNode = nullptr;
			}
			// Один
			else if (pNode->right == nullptr) {
				os << "У " << pNode->value << " только один ЛЕВЫЙ наследник.\n";
				Node* temp = pNode;
				os << pNode->left->value << " заменяет родителя.\n";
				pNode = pNode->left;
				os << pNode->value << " удаляется.\n";
				delete temp;
			}
			else if (pNode->left == nullptr) {
				os << "У " << pNode->value << " только один ПРАВЫЙ наследник.\n";
				Node* temp = pNode;
				os << pNode->right->value << " заменяет родителя.\n";
				pNode = pNode->right;
				os << pNode->value << " удаляется.\n";
				delete temp;
			}
			// Оба
			else {
				os << pNode->value 
					<< " имеет два наследника. Ищем самое правое(наибольшее) значение в ЛЕВОМ поддереве со значением "
					<< pNode->left->value << "\n";
				Node* temp = pNode->left;
				while (temp->right != nullptr)
					temp = temp->right;
				os << "Наибольшее значение - " << temp->value << ".\n";
				os << "Заменяем значение " << pNode->value << " значением " << temp->value << ".\n";
				pNode->value = temp->value;
				os << "Удаляем " << temp->value << " в ЛЕВОМ поддереве со значением " << pNode->left->value << ".\n";
				_DeleteTest(os, pNode->left, temp->value);
			}
		}
	}

	// Вывод с прямым проходом, показался более информативным
	void _Print(const Node* const& pNode, 
				const std::string& prefix = "", 
				bool isLeft = false) const
	{
		if (pNode == nullptr) return;

		//данная конструкция выводится с родной локалью
		std::cout << prefix 
			<< (isLeft ? "\033[90m\xC3\xC4\xC4\xC4\xC4\033[41;97m " : "\033[90m\xC0\xC4\xC4\xC4\xC4\033[42;97m ") 
			<< "" << pNode->value << " \033[0m\n";

		_Print(pNode->left, prefix + ((isLeft) ? "\033[90m|\033[0m     " : "      "), true && pNode->right);
		_Print(pNode->right, prefix + ((isLeft) ? "\033[90m|\033[0m     " : "      "), false);
	}

	// Вывод в файл копипаста без VT100
	void _PrintClean(ostream& os,
					const Node* const& pNode, 
					const std::string& prefix = "", 
					bool isLeft = false) const
	{
		if (pNode == nullptr) return;

		//os << prefix << (isLeft ? "\xC3\xC4\xC4\xC4\xC4[" : "\xC0\xC4\xC4\xC4\xC4[") 
		os << prefix << (isLeft ? "+---Л[" : "+---П[") 
			<< "" << pNode->value << "]\n";

		_PrintClean(os, pNode->left, prefix + ((isLeft) ? "|     " : "      "), true && pNode->right);
		_PrintClean(os, pNode->right, prefix + ((isLeft) ? "|     " : "      "), false);
	}

	void _ForEachForward(Node* const& pNode) const {
		if (pNode == nullptr) return;
		std::cout << '[' << pNode->value << "] ";
		_ForEachForward(pNode->left);
		_ForEachForward(pNode->right);
	}

	void _ForEachBackward(Node* const& pNode) const {
		if (pNode == nullptr) return;
		_ForEachBackward(pNode->left);
		_ForEachBackward(pNode->right);
		std::cout << '[' << pNode->value << "] ";
	}

	Node* root;
};

ostream& operator<<(ostream& os, const BinaryTree& tree) {
	tree.Print();
	return os;
}

bool SetNumbers(vector<int>& numbers) {
	while (true) {
		switch (MenuResult("Как ввести количество элементов?",
			{ "Сгенерировать N чисел", "Вводить вручную", "Считать из файла", "Выход" })) {
		// Сгенерировать N чисел
		case 1: {
			int n = 0;
			while (true) {
				cout << "Введите количество элементов:\nN = ";
				n = Get<int>(cin);
				if (n > 0) break;
				cout << "Некорректный ввод. Повторите!\n\n";
			}
			for (int i = 0; i < n; ++i) {
				numbers.push_back(rnd(199) - rnd(99));
			}
			cout << "\nБыли сгенерированы следующие числа:\n";
			if (numbers.size() < 1000U)
				for (const auto& i : numbers) {
					cout << i << ' ';
				}
			else cout << "Слишком много элементов для вывода.\n";
			cout << "\n";
			return true;
		}
		// Вводить вручную
		case 2: {
			cout << "Вводите поочередно элементы типа int. Размер определяется автоматически"
				"по количеству введенных элементов.\nЗавершение ввода стандартно для консолей"
				"систем Windows (^Z) Ctrl+Z\n";
			for (int i; cin >> i; ) {
				numbers.push_back(i);
			}
			return true;
		}
		// Считать из файла
		case 3: {
			while (true) {
				int ans = MenuResult("Файл должен иметь название numbers.txt и содержать "
					"числа типа int, разделенные пробелом", { "Мне понятно" });
				if (ans != 1) continue;
				ifstream in("numbers.txt");
				if (in) {
					while (in) {
						int i = Get<int>(in);
						numbers.push_back(i);
					}
					numbers.pop_back();
					return true;
				}
			}
		}
		case 4: {
			return false;
		}
		default:
			break;
		}
	}
}

void TreeOutput(const BinaryTree& tree) {
	switch (MenuResult("Вывод дерева", { "Вывод на экран", "Вывод в файл", "Пропустить" })) {
	case 1: return tree.Print();
	case 2: {
		ofstream out("tree.txt", ios::app);
		if (out) {
			out << "\n==========================================================================================\n";
			tree.PrintClean(out);
			out << "\n\n\n";
		}
	}
	case 3:
		cout << '\n';
		break;
	}
}

int main() {
	setlocale(0, "");
	CONSOLE.ActivateWindowsTerminal();
	CONSOLE.FullScreen();

	cout << " ЛАБОРАТОРНАЯ РАБОТА №10\n\n";

	while (true) {
		// 1)
		cout << " ЗАДАНИЕ 1.\n";
		vector<int> numbers;
		if (!SetNumbers(numbers)) break;
		BinaryTree tree;
		{
			LOG_DURATION("Бинарное дерево создано")
			for (const auto& num : numbers) {
				tree.Insert(num);
			}
		}
		MyArray arr;
		{
			LOG_DURATION("Динамический массив создан")
			for (const auto& num : numbers) {
				arr.PushBack(num);
			}
		}
		cout << '\n';

		// 2)
		cout << "\n ЗАДАНИЕ 2.\n";
		TreeOutput(tree);

		// 3)
		cout << "\n ЗАДАНИЕ 3.\n";
		bool run = true;
		while (run) {
			switch (MenuResult("Действия с контейнерами", { "Вставить", "Удалить", "Найти", "Назад" })) {
			case 1: {
				cout << "Введите число типа int для добавления его в контейнер\nNUM = ";
				int num = Get<int>(cin);
				{
					LOG_DURATION("Бинарное дерево. Элемент вставлен")
					tree.Insert(num);
				}
				{
					LOG_DURATION("Динамический массив. Элемент вставлен")
					arr.PushBack(num);
				}
				cout << '\n';
				TreeOutput(tree);
				break;
			}

			case 2: {
				cout << "Введите число типа int для удаления его из контейнера\nNUM = ";
				int num = Get<int>(cin);
				{
					LOG_DURATION("Бинарное дерево. Элемент удален")
					tree.Delete(num);
				}
				{
					LOG_DURATION("Динамический массив. Элемент удален")
					arr.Erase(distance(arr.Begin(), find(arr.Begin(), arr.End(), num)));
				}
				cout << '\n';
				TreeOutput(tree);
				break;
			}

			case 3: {
				cout << "Введите число типа int для поиска его в контейнере\nNUM = ";
				int num = Get<int>(cin);
				Node* p = nullptr;
				{
					LOG_DURATION("Бинарное дерево. Элемент найден")
					p = tree.Find(num);
				}
				{
					LOG_DURATION("Динамический массив. Элемент найден")
					find(arr.Begin(), arr.End(), num);
				}
				cout << '\n';
				if (p) cout << "Адрес ячейки со значением " << num << " - " << p << '\n';
				else cout << "Числа " << num << " нет в контейнере\n";
				cout << '\n';
				break;
			}

			case 4:
				run = false;
				break;
			}

		}
		cout << '\n';

		// 4)
		cout << "\n ЗАДАНИЕ 4.\n";
		cout << "\n\nПрямой обход\n";
		tree.ForEachForward();
		cout << "\n\nОбратный обход\n";
		tree.ForEachBackward();
		cout << "\n\nОбход в ширину\n";
		tree.ForEachWide();

		cout << "\n\n\n";
	}

	// 5)
	cout << "\n ЗАДАНИЕ 5.\n\nГенерирую задания..\n";
	{
		int task = 1;
		{
			// чистим файл ответов
			ofstream replyFile("reply.txt");
		}
		ofstream testFile("test.txt");
		if (testFile) {
			for (int i = 0; i < 3; ++i) {
				BinaryTree tree;
				vector<int> nums;
				for (int a = 0; a < 24; ++a)
					nums.push_back(rnd(100));
				for (const auto& num : nums)
					tree.Insert(num);
				// значение может и не существовать
				int value = rnd(100);
				testFile << "*********************************************************************************\n\n";
				testFile << "Задание " << task << ".\n";
				testFile << "Дано бинарное дерево. Запишите поочередно действия, необходимые для вставки ячейки со значением "
					<< value << ".\n";
				tree.PrintClean(testFile);
				testFile << "\n\n";
				tree.InsertTest(value, task);
				task++;
			}
			for (int i = 0; i < 3; ++i) {
				BinaryTree tree;
				vector<int> nums;
				for (int a = 0; a < 32; ++a)
					nums.push_back(rnd(100));
				for (const auto& num : nums)
					tree.Insert(num);
				// значение может и не существовать
				int value = (rnd(2) ? rnd(100) : nums[rnd(nums.size())]);
				testFile << "*********************************************************************************\n\n";
				testFile << "Задание " << task << ".\n";
				testFile << "Дано бинарное дерево. Запишите поочередно действия, необходимые для удаления ячейки со значением "
					<< value << ".\n";
				tree.PrintClean(testFile);
				testFile << "\n\n";
				tree.DeleteTest(value, task);
				task++;
			}
		}
	}
	cout << "Готово. Файлы \"test.txt\" и \"reply.txt\" лежат в каталоге программы.\n\n\n";
}