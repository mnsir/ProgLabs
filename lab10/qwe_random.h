#pragma once

#include <chrono>
#include <random>

// ================================================================================================
// рандом
template<typename T>
T rnd(const T& border) {
	const auto t = std::chrono::system_clock::now().time_since_epoch().count();
	std::minstd_rand gen(static_cast<uint32_t>(t));
	return gen() % border;
}
// ================================================================================================
