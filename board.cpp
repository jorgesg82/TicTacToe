#include "board.h"

const std::array<std::array<Pair, 3>, 8> Board::winPosibilities =
    {{// Horizontal
      {Pair(0, 0), Pair(0, 1), Pair(0, 2)},
      {Pair(1, 0), Pair(1, 1), Pair(1, 2)},
      {Pair(2, 0), Pair(2, 1), Pair(2, 2)},

      // Vertical
      {Pair(0, 0), Pair(1, 0), Pair(2, 0)},
      {Pair(0, 1), Pair(1, 1), Pair(2, 1)},
      {Pair(0, 2), Pair(1, 2), Pair(2, 2)},

      // Diagonal
      {Pair(0, 0), Pair(1, 1), Pair(2, 2)},
      {Pair(0, 2), Pair(1, 1), Pair(2, 0)}}};

Board::Board()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; j++)
        {
            boxes[i][j] = box::VOID;
        }
    }
}

int Board::isWin() const
{
    // Check if win
    for (const auto winPosibility : winPosibilities)
    {
        if (getValue(winPosibility[0]) == getValue(winPosibility[1]) && getValue(winPosibility[1]) == getValue(winPosibility[2]))
        {
            if (getValue(winPosibility[0]) == box::X)
                return 1; // Player1 wins
            if (getValue(winPosibility[0]) == box::O)
                return 2; // Player2 wins
        }
    }
    // Check if draw
    for (const auto row : boxes)
    {
        for (const box b : row)
        {
            if (b == box::VOID)
            {
                return -1;
            }
        }
    }
    return 0;
}

box Board::getValue(const Pair &pair) const
{
    return boxes[pair.first][pair.second];
}

void Board::setValue(const Pair &pair, const box& value)
{
    boxes[pair.first][pair.second] = value;
}