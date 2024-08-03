#ifndef BOARD_H
#define BOARD_H

#include "utils.h"

class Board
{
public:
    Board();

    /// @brief
    /// @return -1 if game continues; 0 if draw; 1 if player1 wins; 2 if player2 wins
    int isWin() const;
    box getValue(const Pair& pair) const;
    void setValue(const Pair& pair, const box& value);

private:
    static const std::array<std::array<Pair, 3>, 8> winPosibilities;

    std::array<std::array<box, 3>, 3> boxes;
};

#endif