#pragma once

#include <fstream>
#include <string>
#include <iterator>

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
	}

	void FillByFile(const std::string& name) {
		Resize(0U);
		std::ifstream input(name);
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
	std::copy(arr.Begin(), arr.End(), std::ostream_iterator<int>(os, ", "));
	return os << " size: " << arr.Size();
}