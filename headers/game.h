#ifndef GAME_H
#define GAME_H

#include <string>
#include "ia.h"
#include <thread>

using namespace std;

enum direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Game
{
public:
    Game(const string &player1);
    Game(const string &player1, const string &player2);

    int startGame();

private:
    static const array<array<pair<int, int>, 10>, 4> directions;

    Board gameBoard;
    Board tempBoard;

    bool turn;
    int turnCount;
    bool iaPlayer;

    string player1;
    optional<string> player2;
    IA ia;

    thread drawer;
    mutex mtxGameBoard;
    mutex mtxTempBoard;
    atomic<bool> end;

    Pair tempPos;

    void changePos(const direction &dir);
    void drawBoard(const Board &board) const;
    void drawGame();
};

#endif