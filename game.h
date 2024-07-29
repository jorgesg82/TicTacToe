#ifndef GAME_H
#define GAME_H

#include <string>

enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

/// @brief 
/// @param player1 
/// @param player2 
/// @return -1 if draw, 0 if player1 wins, 1 if player2 wins
int startGame(const std::string &player1, const std::string &player2);

#endif