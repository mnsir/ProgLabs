#pragma once

#include <ios>

template<class T>
T Get(std::istream& is) {
	T x;
	is >> x;
	return x;
}