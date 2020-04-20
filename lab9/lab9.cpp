#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include "conapi.h"
#include "Student.h"
#include "tmanip.h"
#include "table_maker.h"
#include "Student.h"
#include <functional>
#include <queue>

extern Console con;

void EditStudent(shared_ptr<Student>& s) {
	// устанавливаем размеры табера
	constexpr int OffsetX = 27;
	constexpr int OffsetY = 12;

	int x0 = con.CX() - OffsetX;
	int x1 = con.CX() + OffsetX;
	int y0 = con.CY() - OffsetY;
	int y1 = con.CY() + OffsetY;

	Taber taber(
		(s->FullName().empty() ? "Новый студент"_s : ("Студент " + s->FullName())),
		x0, y0, x1, y1);

	x0 += 3;
	y0 += 4;
	taber.Add(make_shared<Edit>("       Фамилия"_s, x0, y0 + 0, s->LastName, 20, isrus));
	taber.Add(make_shared<Edit>("           Имя"_s, x0, y0 + 2, s->FirstName, 20, isrus));
	taber.Add(make_shared<Edit>("      Отчество"_s, x0, y0 + 4, s->SecondName, 20, isrus));
	taber.Add(make_shared<Selector<Sex>>("           Пол"_s, x0, y0 + 6, s->Sex));
	taber.Add(make_shared<Selector<Form>>("Форма обучения"_s, x0, y0 + 8, s->Form));
	taber.Add(make_shared<Edit>("  Номер группы"_s, x0, y0 + 10, s->GroupID, 4, isdigit));

	x0 += 37;
	taber.Add(make_shared<Selector<Skin2>>("ВВвИТ"_s, x0, y0 + 0, s->Exam[0]));
	taber.Add(make_shared<Selector<Skin2>>("Матан"_s, x0, y0 + 2, s->Exam[1]));
	taber.Add(make_shared<Selector<Skin2>>("А и Г"_s, x0, y0 + 4, s->Exam[2]));
	taber.Add(make_shared<Selector<Skin2>>("Истор"_s, x0, y0 + 6, s->Credit[0]));
	taber.Add(make_shared<Selector<Skin2>>("Ин Яз"_s, x0, y0 + 8, s->Credit[1]));
	taber.Add(make_shared<Selector<Skin2>>("Прог1"_s, x0, y0 + 10, s->Credit[2]));
	taber.Add(make_shared<Selector<Skin2>>("Прог2"_s, x0, y0 + 12, s->Credit[3]));
	taber.Add(make_shared<Selector<Skin2>>("Филос"_s, x0, y0 + 14, s->Credit[4]));

	taber.Add(make_shared<Button>("Ok", x1 - ButtonWidth - ButtonWidth, y1 - ButtonHeight + 1));
	taber.Add(make_shared<CancelButton>("Cancel", x1 - ButtonWidth, y1 - ButtonHeight + 1));

	taber.Action();	//вызываем табер
	s->SetEditDate();
}

class MyDataBase : public TableUser, public MenuUser {
public:
	MyDataBase(const string& Database)
		: setws(GetHeaderFieldCount())
		, list(nullptr)
	{
		ifstream file(Database);
		if (file) {
			string str;
			getline(file, str);
			lastID = stoi(str);
			Student student;
			while (file >> student) {
				students.push_back(make_shared<Student>(student));
			}
		}
		SetwsInit();
		UpdateSetws();
	}




// TableUser =============================
	string GetHeader() const override {
		stringstream ss;
		ss << left;
		size_t n = GetHeaderFieldCount();
		for (size_t i = 0; i < n; ++i) {
			ss << setw(setws[i]) << GetHeaderField(i);
		}
		return ss.str();
	}

	size_t GetRecordCount() const override {
		return students.size();
	}

	string GetRecord(const size_t& num) const override {
		stringstream ss;
		ss << left;
		size_t n = GetHeaderFieldCount();
		for (size_t i = 0; i < n; ++i) {
			ss << setw(setws[i]) << students[num]->GetField(i);
		}
		return ss.str();
	}

	/*void MakeFriend(Table* list_) override {
		list - list_;
	}*/

	size_t GetHeaderFieldCount() const { return 17U; }

	string GetHeaderField(const size_t& i) const {
		switch (i) {
		case 0U: return "ID"_s;
		case 1U: return "Фамилия"_s;
		case 2U: return "Имя"_s; 
		case 3U: return "Отчество"_s; 
		case 4U: return "Пол"_s; 
		case 5U: return "Форма"_s; 
		case 6U: return "Группа"_s; 
		case 7U: return "Номер"_s;
		case 8U: return "ВВвИТ"_s;
		case 9U: return "Матан"_s; 
		case 10U: return "А_и_Г"_s; 
		case 11U: return "Истор"_s; 
		case 12U: return "Ин_Яз"_s; 
		case 13U: return "Прог1"_s; 
		case 14U: return "Прог2"_s; 
		case 15U: return "Филос"_s; 
		case 16U: return "Изменен"_s;
		default: return "error!";
		}
	}

	void SetwsInit() {
		size_t n = GetHeaderFieldCount();
		for (size_t i = 0; i < n; ++i) {
			setws[i] = GetHeaderField(i).size() + 1U;
		}
	}

	void UpdateSetws() {
		size_t n = GetHeaderFieldCount();
		for (const auto& s : students) {
			for (size_t i = 0; i < n; ++i) {
				string temp = s->GetField(i);
				if (temp.size() >= setws[i])
					setws[i] = temp.size() + 1U;
			}
		}
	}

	void UpdateSetwsForNew(shared_ptr<Student>& s) {
		size_t n = GetHeaderFieldCount();
		for (size_t i = 0; i < n; ++i) {
			string temp = s->GetField(i);
			if (temp.size() >= setws[i])
				setws[i] = temp.size() + 1U;
		}
	}

// ==============================================
// ==============================================
// Menu Methods =================================

	void Add() {
		auto s = make_shared<Student>();
		EditStudent(s);
		s->ID = ++lastID;
		UpdateSetwsForNew(s);
		students.push_back(s);
		if (list) list->Update();
	}

	void Edit() {
		if (list) {
			list->Action();
			const size_t& i = list->GetCurrent();
			EditStudent(students[i]);
			UpdateSetwsForNew(students[i]);
			list->Update();
		}
	}

// MenuUser =====================================

	size_t GetMenuItemCount() const override {
		return 2U;
	}

	string GetMenuItemName(const size_t& i) override {
		switch (i) {
		case 0U: return "Создать"_s;
		case 1U: return "Редактировать"_s;
		default: return "error!";
		}
	}

	void RunMenuItem(const size_t& i) override {
		switch (i) {
		case 0U: Add();
			break;
		case 1U: Edit();
			break;
		default:
			break;
		}
	}
// ==============================================

private:
	vector<shared_ptr<Student>> students;
	vector<streamsize> setws;
	int lastID;
	Table* list;
};

using namespace std;

int main() {
	setlocale(0, "C");

	con.ActivateWindowsTerminal();
	con.FullScreen();
	cout << hide;


	auto base = make_shared<MyDataBase>("base.txt");
	

	Taber taber("Лабораторная работа №9"_s, 1, 1, con.X(), con.Y());
	taber.Add(make_shared<Menu>("Действия:"_s, con.X() - 15, 5, base));
	taber.Add(make_shared<Table>("Информация о студентах:"_s, 3, 5, base));
	taber.Action();
	/*cout << set(90, 20) << "\033[40m \033[41m \033[42m \033[43m \033[44m \033[45m \033[46m \033[47m \n"
		<< set(90, 21) << "\033[100m \033[101m \033[102m \033[103m \033[104m \033[105m \033[106m \033[107m \033[0m";*/

	auto s = make_shared<Student>();
	EditStudent(s);

	if (true) {

	}
	else {

	}


	/*auto name = Input<Sex>(5, 5);
	auto name1 = Input<Form>(5, 6);
	auto name2 = Input<Skin2>(5, 7);
	auto name3 = Input<string>(5, 8);*/
	Sleep(5000);
	return 0;

	// база
	//DB db;
	//db.Redraw(1, 5);

	// меню
	vector<string> v = {
		"пункт 1",
		"пункт 2",
		"пункт 3 123",
	};

	/*Menu menu(v);
	menu.Redraw(150, 5);
	menu.Skin2(1);*/



	cout << set(0, 30);
}