#pragma once
// 
// 
// 
// ================================================================================================
// manipulators

// Перемещает курсор на steps шагов вверх
std::string up(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'A';
    return ss.str();
}
// Перемещает курсор на steps шагов вниз
std::string down(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'B';
    return ss.str();
}
// Перемещает курсор на steps шагов вправо
std::string right(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'C';
    return ss.str();
}
// Перемещает курсор на steps шагов влево
std::string left(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'D';
    return ss.str();
}

// Перемещает курсор на steps строк вниз в начало строки
std::string down_line(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'E';
    return ss.str();
}
// Перемещает курсор на steps строк вверх в начало строки
std::string up_line(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'F';
    return ss.str();
}

// Перемещает курсор в позицию X строки
std::string setcol(const int& X = 1) {
    std::stringstream ss;
    ss << "\033[" << X << 'G';
    return ss.str();
}
// Перемещает курсор в строку - Y, позицию - X
std::string set(const int& X = 1, const int& Y = 1) {
    std::stringstream ss;
    ss << "\033[" << Y << ';' << X << 'H';
    return ss.str();
}

// Очищает экран: 0 - от курсора, 1 - до курсора, 2 - весь
std::string erase(const int& arg = 2) {
    std::stringstream ss;
    ss << "\033[" << arg << 'J';
    return ss.str();
}
// Очищает строку: 0 - от курсора, 1 - до курсора, 2 - всю
std::string erase_line(const int& arg = 2) {
    std::stringstream ss;
    ss << "\033[" << arg << 'K';
    return ss.str();
}

// Прокручивает страницу вверх на lines строк
std::string scroll_up(const int& lines = 1) {
    std::stringstream ss;
    ss << "\033[" << lines << 'S';
    return ss.str();
}
// Прокручивает страницу вниз на lines строк
std::string scroll_down(const int& lines = 1) {
    std::stringstream ss;
    ss << "\033[" << lines << 'T';
    return ss.str();
}

// Сохраняет положение курсора
std::ostream& save(std::ostream& os) { return os << "\033[s"; }
// Восстанавливает положение курсора
std::ostream& load(std::ostream& os) { return os << "\033[s"; }

// Скрывает курсор
std::ostream& hide(std::ostream& os) { return os << "\033[?25l"; }
// Делает курсор видимым
std::ostream& show(std::ostream& os) { return os << "\033[?25h"; }

// ==============
// Прокручивает страницу вниз на lines строк
std::string invert(const bool& enable = true) {
    if (enable) return "\033[7m";
    return "\033[27m";
}

// Подчеркнуть
std::string underline(const bool& enable = true) {
    if (enable) return "\033[4m";
    return "\033[24m";
}

// Подчеркнуть
std::ostream& underline(std::ostream& os) { return os << underline(); }

// Очищает символы, заменяя их пробелами:
std::string erase_chars(const int& count = 1) {
    std::stringstream ss;
    ss << "\033[" << count << 'X';
    return ss.str();
}
