#include <iostream>
#include <memory>
#include <vector>
#include <deque>
#include <conio.h>
#include "qwe_console.h"

#define SAVE			"\033[s"			//Сохраняет положение курсора
#define LOAD			"\033[u"			//Восстанавливает положение курсора

// Для _kbhit
constexpr int KB_0         = 48;
constexpr int KB_1         = 49;
constexpr int KB_2         = 50;
constexpr int KB_3         = 51;
constexpr int KB_4         = 52;
constexpr int KB_5         = 53;
constexpr int KB_6         = 54;
constexpr int KB_7         = 55;
constexpr int KB_8         = 56;
constexpr int KB_9         = 57;
constexpr int KB_Enter     = 13;
constexpr int KB_ESC       = 27;
constexpr int KB_TAB       =  9;
constexpr int KB_Space     = 32;
constexpr int KB_Backspace =  8;
constexpr int KB_Up        = 72;
constexpr int KB_Down      = 80;
constexpr int KB_Left      = 75;
constexpr int KB_Right     = 77;

constexpr short OBJ_SIZE = 5i16;

short BORDER_UP    = 0;
short BORDER_RIGHT = 0;
short BORDER_DOWN  = 0;
short BORDER_LEFT  = 0;

Console CONSOLE;

using namespace std;

enum class Currency {
    Down,
    Right,
    Up,
    Left,
};

class Bullet;

deque<shared_ptr<Bullet>> BULLETS;

class Bullet {
public:
    Bullet(const short& x, const short& y, const Currency& currency)
        : _X(x)
        , _Y(y)
        , _currency(currency)
    {
    }

    void Draw() {
        CONSOLE.WriteChar('0', _X, _Y);
    }

    void Move() {
        CONSOLE.WriteChar(' ', _X, _Y);
        switch (_currency) {
        case Currency::Down:
            ++_Y;
            break;
        case Currency::Right:
            ++_X;
            break;
        case Currency::Up:
            --_Y;
            break;
        case Currency::Left:
            --_X;
            break;
        default: break;
        }
    }

    int GetX() const { return _X; }
    int GetY() const { return _Y; }
    Currency GetCurrency() const { return _currency; }

private:
    short _X;
    short _Y;
    Currency _currency;
};


const char* tankDown[OBJ_SIZE] = {
    {"\xD6\x2F\xC4\x5C\xB7" },
    {"\xCC\xDA\xC4\xBF\xB9" },
    {"\xCC\xB3\xFD\xB3\xB9" },
    {"\xCC\xC0\xBA\xD9\xB9" },
    {"\xD3\x5C\xBA\x2F\xBD" },
};
const char* tankUp[OBJ_SIZE] = {
    {"\xD6\x2F\xBA\x5C\xB7" },
    {"\xCC\xDA\xBA\xBF\xB9" },
    {"\xCC\xB3\xFD\xB3\xB9" },
    {"\xCC\xC0\xC4\xD9\xB9" },
    {"\xD3\x5C\xC4\x2F\xBD" },
};
const char* tankRight[OBJ_SIZE] = {
    { "\xD5\xCB\xCB\xCB\xB8" },
    { "\x2F\xDA\xC4\xBF\x5C" },
    { "\xB3\xB3\xFD\xCD\xCD" },
    { "\x5C\xC0\xC4\xD9\x2F" },
    { "\xD4\xCA\xCA\xCA\xBE" },
};
const char* tankLeft[OBJ_SIZE] = {
    { "\xD5\xCB\xCB\xCB\xB8" },
    { "\x2F\xDA\xC4\xBF\x5C" },
    { "\xCD\xCD\xFD\xB3\xB3" },
    { "\x5C\xC0\xC4\xD9\x2F" },
    { "\xD4\xCA\xCA\xCA\xBE" },
};

//const char brick[OBJ_SIZE][OBJ_SIZE] = {
//    {'\xCD', '\xCB', '\xCD', '\xCD', '\xCA' },
//    {'\xCD', '\xCA', '\xCD', '\xCB', '\xCD' },
//    {'\xCB', '\xCD', '\xCD', '\xCA', '\xCD' },
//    {'\xCA', '\xCD', '\xCB', '\xCD', '\xCD' },
//    {'\xCD', '\xCD', '\xCA', '\xCD', '\xCB' },
//};

const char* brick[OBJ_SIZE] = {
    { "\xCB\xCA\xCB\xCD\xCA" },
    { "\xCA\xCB\xCA\xCB\xCD" },
    { "\xCD\xCA\xCB\xCA\xCB" },
    { "\xCB\xCD\xCA\xCB\xCA" },
    { "\xCA\xCB\xCD\xCA\xCB" },
};

class Tank {
public:
    Tank(const short& x, const short& y, const Currency& currency)
        : _X(x)
        , _Y(y)
        , _currency(currency)
    {
        Draw();
    }

    void Draw() {
        for (short y = 0; y < OBJ_SIZE; ++y) {
            CONSOLE.SetCursorPosition(_X, _Y + y);
            switch (_currency) {
            case Currency::Down:
                CONSOLE.Write(tankDown[y]);
                break;
            case Currency::Right:
                CONSOLE.Write(tankRight[y]);
                break;
            case Currency::Up:
                CONSOLE.Write(tankUp[y]);
                break;
            case Currency::Left:
                CONSOLE.Write(tankLeft[y]);
                break;
            default:
                break;
            }
        }
    }

    void Turn(const Currency& currency) { _currency = currency; }

    void Move() {
        deque<pair<int, int>> front = GetFront();

        switch (_currency) {
        case Currency::Down:
            if (_Y < BORDER_DOWN - OBJ_SIZE) {
                CONSOLE.SetCursorPosition(_X, _Y);
                CONSOLE.Write("     ");
                _Y++;
            }
            break;
        case Currency::Right:
            if (_X < BORDER_RIGHT - OBJ_SIZE) {
                for (short i = 0; i < OBJ_SIZE; ++i) {
                    CONSOLE.WriteChar(' ', _X, _Y + i);
                }
                _X++;
            }
            break;
        case Currency::Up:
            if (_Y > BORDER_UP) {
                CONSOLE.SetCursorPosition(_X, _Y + 4);
                CONSOLE.Write("     ");
                _Y--;
            }
            break;
        case Currency::Left:
            if (_X > BORDER_LEFT) {
                for (short i = 0; i < OBJ_SIZE; ++i) {
                    CONSOLE.WriteChar(' ', _X + 4, _Y + i);
                }
                _X--;
            }
            break;
        default:
            break;
        }
    }

    void Fire() const {
        switch (_currency) {
        case Currency::Down:
            BULLETS.push_back(make_shared<Bullet>(_X + 2, _Y + OBJ_SIZE - 1, _currency));
            break;
        case Currency::Right:
            BULLETS.push_back(make_shared<Bullet>(_X + OBJ_SIZE - 1, _Y + 2, _currency));
            break;
        case Currency::Up:
            BULLETS.push_back(make_shared<Bullet>(_X + 2, _Y, _currency));
            break;
        case Currency::Left:
            BULLETS.push_back(make_shared<Bullet>(_X, _Y + 2, _currency));
            break;
        default:
            break;
        }
    }

    deque<pair<int, int>> GetFront() const {
        deque<pair<int, int>> front;
        int x = 0, y = 0;
        for (int i = 0; i < OBJ_SIZE; ++i) {
            switch (_currency) {
            case Currency::Down:
                x = _X + i;
                y = _Y + OBJ_SIZE;
                break;
            case Currency::Right:
                x = _X + OBJ_SIZE;
                y = _Y + i;
                break;
            case Currency::Up:
                x = _X + i;
                y = _Y - 1;
                break;
            case Currency::Left:
                x = _X - 1;
                y = _Y + i;
                break;
            default:
                break;
            }
            //CONSOLE.WriteChar('x', x, y); //DEBUG
            front.push_back(make_pair(x, y));
        }
        //Sleep(10); //DEBUG
        return front;
    }

    int GetX0() const { return _X; }
    int GetY0() const { return _Y; }
    Currency GetCurrency() const { return _currency; }

private:
    short _X;
    short _Y;
    Currency _currency;
};

class Brick {
public:
    Brick(const short& x, const short& y)
        : _X(x)
        , _Y(y)
    {
        for (auto& row : health)
            for (auto& item : row)
                item = true;
        Draw();
    }

    void Draw() {
        for (short y = 0; y < OBJ_SIZE; ++y) {
            CONSOLE.SetCursorPosition(_X, _Y + y);
            CONSOLE.Write(brick[y]);
        }
    }

    bool ShotBy(const shared_ptr<Bullet>& pBullet) {
        int x = pBullet->GetX();
        int y = pBullet->GetY();
        if (x >= _X && x < _X + OBJ_SIZE && y >= _Y && y < _Y + OBJ_SIZE) {
            if (health[x - _X][y - _Y]) {
                CONSOLE.WriteChar(' ', x, y);
                health[x - _X][y - _Y] = false;
                return true;
            }
            //switch (pBullet->GetCurrency()) {
            //case Currency::Down:
            //case Currency::Up:
            //    break;
            //case Currency::Right:
            //case Currency::Left:
            //    break;
            //default: break;
            //}
        }
        return false;
    }

    bool Contains(const int& x, const int& y) const {
        if (x >= _X && x < _X + OBJ_SIZE && y >= _Y && y < _Y + OBJ_SIZE) {
            //CONSOLE.WriteChar(health[x - _X][y - _Y] ? '+' : '-', x, y);
            return health[x - _X][y - _Y];
        }
        return false;
    }

private:
    short _X;
    short _Y;

    bool health[OBJ_SIZE][OBJ_SIZE];
    //vector<bool> health;
};

void DrawBrick() {
    cout << "\033[48;2;191;89;52m\033[38;2;115;57;44m";
    for (int y = 0; y < OBJ_SIZE; ++y) {
        for (int x = 0; x < OBJ_SIZE; ++x)
            cout << brick[y];
        cout << '\n';
    }
    cout << "\033[0m";
}

bool CanMove(const Tank& tank, const shared_ptr<Brick>& pBrick) {
    auto face = tank.GetFront();
    while (!face.empty()) {
        if (pBrick->Contains(face.front().first, face.front().second)) return false;
        face.pop_front();
    }
    return true;
}

bool CanMove(const Tank& tank, const vector<shared_ptr<Brick>>& bricks) {
    for (const auto& brick : bricks) {
        if (!CanMove(tank, brick)) return false;
    }
    return true;
}

int main() {
    CONSOLE.SetFontSize(24i16, 24i16);
    CONSOLE.ActivateWindowsTerminal();
    CONSOLE.FullScreen();

    CONSOLE.DrawFrame();

    BORDER_UP = 3;
    BORDER_RIGHT = CONSOLE.GetWidth() - 1i16;
    BORDER_DOWN = CONSOLE.GetHeight() - 1i16;
    BORDER_LEFT = 1;

    Tank tank(5, 5, Currency::Down);

    vector<shared_ptr<Brick>> bricks;
    bricks.push_back(make_shared<Brick>(10, 10));
    bricks.push_back(make_shared<Brick>(15, 10));
    bricks.push_back(make_shared<Brick>(15, 15));
    
    bool run = true;
    while (run) {

        deque<shared_ptr<Bullet>> buffer;
        while (!BULLETS.empty()) {
            auto p = BULLETS.front();
            BULLETS.pop_front();
            p->Move();
            if (p->GetX() < BORDER_RIGHT && p->GetX() >= BORDER_LEFT &&
                p->GetY() < BORDER_DOWN && p->GetY() >= BORDER_UP) {
                bool flag = false;
                for (const auto& pBrick : bricks) {
                    if (pBrick->ShotBy(p)) {
                        flag = true;
                        break;
                    }
                }
                if (flag) continue;
                p->Draw();
                buffer.push_back(p);
            }
        }
        BULLETS = buffer;

        if (_kbhit()) {
            int ch = _getch();
            switch (ch) {
            case KB_Space:
                tank.Fire();
                break;
            case KB_Up:
                tank.Turn(Currency::Up);
                if (CanMove(tank, bricks)) tank.Move();
                tank.Draw();
                break;
            case KB_Right:
                tank.Turn(Currency::Right);
                if (CanMove(tank, bricks)) tank.Move();
                tank.Draw();
                break;
            case KB_Down:
                tank.Turn(Currency::Down);
                if (CanMove(tank, bricks)) tank.Move();
                tank.Draw();
                break;
            case KB_Left:
                tank.Turn(Currency::Left);
                if (CanMove(tank, bricks)) tank.Move();
                tank.Draw();
                break;
            default:
                break;
            }
            // DEBUG
            CONSOLE.SetCursorPosition(1, 1);
            CONSOLE.Write(("pos x: " + to_string(tank.GetX0()) + " pos y: " + to_string(tank.GetY0())).c_str());
        }


        Sleep(50);

    }


    
}
