#pragma once
// 
// 
// 
// ================================================================================================
// manipulators

// ���������� ������ �� steps ����� �����
std::string up(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'A';
    return ss.str();
}
// ���������� ������ �� steps ����� ����
std::string down(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'B';
    return ss.str();
}
// ���������� ������ �� steps ����� ������
std::string right(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'C';
    return ss.str();
}
// ���������� ������ �� steps ����� �����
std::string left(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'D';
    return ss.str();
}

// ���������� ������ �� steps ����� ���� � ������ ������
std::string down_line(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'E';
    return ss.str();
}
// ���������� ������ �� steps ����� ����� � ������ ������
std::string up_line(const int& steps = 1) {
    std::stringstream ss;
    ss << "\033[" << steps << 'F';
    return ss.str();
}

// ���������� ������ � ������� X ������
std::string setcol(const int& X = 1) {
    std::stringstream ss;
    ss << "\033[" << X << 'G';
    return ss.str();
}
// ���������� ������ � ������ - Y, ������� - X
std::string set(const int& X = 1, const int& Y = 1) {
    std::stringstream ss;
    ss << "\033[" << Y << ';' << X << 'H';
    return ss.str();
}

// ������� �����: 0 - �� �������, 1 - �� �������, 2 - ����
std::string erase(const int& arg = 2) {
    std::stringstream ss;
    ss << "\033[" << arg << 'J';
    return ss.str();
}
// ������� ������: 0 - �� �������, 1 - �� �������, 2 - ���
std::string erase_line(const int& arg = 2) {
    std::stringstream ss;
    ss << "\033[" << arg << 'K';
    return ss.str();
}

// ������������ �������� ����� �� lines �����
std::string scroll_up(const int& lines = 1) {
    std::stringstream ss;
    ss << "\033[" << lines << 'S';
    return ss.str();
}
// ������������ �������� ���� �� lines �����
std::string scroll_down(const int& lines = 1) {
    std::stringstream ss;
    ss << "\033[" << lines << 'T';
    return ss.str();
}

// ��������� ��������� �������
std::ostream& save(std::ostream& os) { return os << "\033[s"; }
// ��������������� ��������� �������
std::ostream& load(std::ostream& os) { return os << "\033[s"; }

// �������� ������
std::ostream& hide(std::ostream& os) { return os << "\033[?25l"; }
// ������ ������ �������
std::ostream& show(std::ostream& os) { return os << "\033[?25h"; }

// ==============
// ������������ �������� ���� �� lines �����
std::string invert(const bool& enable = true) {
    if (enable) return "\033[7m";
    return "\033[27m";
}

// �����������
std::string underline(const bool& enable = true) {
    if (enable) return "\033[4m";
    return "\033[24m";
}

// �����������
std::ostream& underline(std::ostream& os) { return os << underline(); }

// ������� �������, ������� �� ���������:
std::string erase_chars(const int& count = 1) {
    std::stringstream ss;
    ss << "\033[" << count << 'X';
    return ss.str();
}
