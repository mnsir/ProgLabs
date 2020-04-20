#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <conio.h>
#include "conapi.h"
#include <Windows.h>
#include "tmanip.h"

// это костыль дл€ русских буков
std::string operator"" _s(const char* str, size_t size)
{
	char buf[128];
	CharToOemA(str, buf);
	return std::string(buf, size);
}

constexpr int kb_backspace = 8;
constexpr int kb_tab = 9;
constexpr int kb_enter = 13;
constexpr int kb_up = 72;
constexpr int kb_left = 75;
constexpr int kb_right = 77;
constexpr int kb_down = 80;

using namespace std;

// ================================================================================================


// ================================================================================================

enum class Sex : size_t {
	male = 0U,
	female,
	unknown,
};

string to_string(const Sex& sex) {
	string str;
	switch (sex) {
	case Sex::male:		return "мужской"_s;
	case Sex::female:	return "женский"_s;
	default:			return "-";
	}
}

istream& operator>>(istream& is, Sex& sex) {
	string str;
	is >> str;
	if (str == "мужской"_s) sex = Sex::male;
	else if (str == "женский"_s) sex = Sex::female;
	else sex = Sex::unknown;
	return is;
}

ostream& operator<<(ostream& os, const Sex& sex) {
	return os << to_string(sex);
}

// ================================================================================================

enum class Form : size_t {
	fulltime,
	evening,
	correspondence,
	unknown,
};

string to_string(const Form& sex) {
	switch (sex) {
	case Form::fulltime:		return "очна€"_s;
	case Form::evening:			return "вечерн€€"_s;
	case Form::correspondence:	return "заочна€"_s;
	default:					return "-";
	}
}

istream& operator>>(istream& is, Form& form) {
	string str;
	is >> str;
	if (str == "очна€"_s) form = Form::fulltime;
	else if (str == "вечерн€€"_s) form = Form::evening;
	else if (str == "заочна€"_s) form = Form::correspondence;
	else form = Form::unknown;
	return is;
}

ostream& operator<<(ostream& os, const Form& form) {
	return os << to_string(form);
}

// ================================================================================================

enum class Skin2 : size_t {
	two,
	three,
	four,
	five,
	unknown,
};

string to_string(const Skin2& mark) {
	switch (mark) {
	case Skin2::five:	return "отл"_s;
	case Skin2::four:	return "хор"_s;
	case Skin2::three:	return "уд"_s;
	case Skin2::two:		return "неуд"_s;
	default:			return "-";
	}
}

istream& operator>>(istream& is, Skin2& form) {
	string str;
	is >> str;
	if (str == "отл"_s) form = Skin2::five;
	else if (str == "хор"_s) form = Skin2::four;
	else if (str == "уд"_s) form = Skin2::three;
	else if (str == "неуд"_s) form = Skin2::two;
	else form = Skin2::unknown;
	return is;
}

ostream& operator<<(ostream& os, const Skin2& mark) {
	return os << to_string(mark);
}

// ================================================================================================

string Today() {
	constexpr size_t size = 20U;
	CHAR date[size];
	GetDateFormatA(LOCALE_SYSTEM_DEFAULT, 0, NULL, "dd-MMMM-yyyy", date, size);
	return date;
}

struct Student {
	Student()
	{
	}

	size_t ID;				//ID

	string LastName;		//‘»ќ
	string FirstName;
	string SecondName;
	Sex Sex;				//пол

	Form Form;				//форма обучени€
	string GroupID;			//номер группы
	size_t ListNumber;		//номер в списке группы
	Skin2 Exam[3];			//оценки за прошедшую сессию(всего 3 экзамена и 5 дифф.зачЄтов)
	Skin2 Credit[5];

	string editDate;		//отметка времени о внесении или изменении данных

	void SetEditDate() { editDate = Today(); }

	string FullName() const {
		return LastName.empty() ? "" : LastName + ' ' + FirstName[0] + ". " + SecondName[0] + '.';
	}

	string GetField(const size_t& i) const {
		switch (i) {
		case 0U: return to_string(ID);
		case 1U: return LastName;
		case 2U: return FirstName;
		case 3U: return SecondName;
		case 4U: return to_string(Sex);
		case 5U: return to_string(Form);
		case 6U: return GroupID;
		case 7U: return to_string(ListNumber);
		case 8U: return to_string(Exam[0]);
		case 9U: return to_string(Exam[1]);
		case 10U: return to_string(Exam[2]);
		case 11U: return to_string(Credit[0]);
		case 12U: return to_string(Credit[1]);
		case 13U: return to_string(Credit[2]);
		case 14U: return to_string(Credit[3]);
		case 15U: return to_string(Credit[4]);
		case 16U: return editDate;
		default: return "error!";
		}
	}
};

bool operator<(const Student& lhs, const Student& rhs) {
	return tie(lhs.LastName, lhs.FirstName, lhs.SecondName) <
		tie(rhs.LastName, rhs.FirstName, rhs.SecondName);
}

istream& operator>>(istream& is, Student& student) {
	string str;
	getline(is, str);
	istringstream iss(str);
	iss >> student.ID
		>> student.LastName
		>> student.FirstName
		>> student.SecondName
		>> student.Sex
		>> student.Form
		>> student.GroupID
		>> student.ListNumber
		>> student.Exam[0]
		>> student.Exam[1]
		>> student.Exam[2]
		>> student.Credit[0]
		>> student.Credit[1]
		>> student.Credit[2]
		>> student.Credit[3]
		>> student.Credit[4]
		>> student.editDate;
	return is;
}

ostream& operator<<(ostream& os, const Student& student) {
	os << left << setw(3) << student.ID
		<< setw(20) << student.LastName
		<< setw(20) << student.FirstName
		<< setw(20) << student.SecondName
		<< setw(8) << student.Sex
		<< setw(9) << student.Form
		<< setw(5) << student.GroupID
		<< setw(3) << student.ListNumber
		<< setw(5) << student.Exam[0]
		<< setw(5) << student.Exam[1]
		<< setw(5) << student.Exam[2]
		<< setw(5) << student.Credit[0]
		<< setw(5) << student.Credit[1]
		<< setw(5) << student.Credit[2]
		<< setw(5) << student.Credit[3]
		<< setw(5) << student.Credit[4]
		<< student.editDate;
	return os;
}

