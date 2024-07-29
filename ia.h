#ifndef IA_H
#define IA_H

#include <array>

#include "utils.h"

class Node {
public:
    Node(Board gameStatus) {
        this->gameStatus = gameStatus;
    }
    int evaluate();

private:
    Board gameStatus;
};

#endif