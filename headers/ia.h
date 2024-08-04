#ifndef IA_H
#define IA_H

#include "utils.h"
#include "board.h"
#include <vector>

class Node
{
public:
    Node(const Board& gameStatus, const bool& max);

    friend class IA;

private:
    int evaluate() const;
    Board gameStatus;
    bool max;
    std::vector<Node> childs;

    bool equals(const Pair &pair1, const Pair &pair2) const;
};

class IA
{
public:
    Pair play(const Board &board) const;
};

#endif