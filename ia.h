#ifndef IA_H
#define IA_H

#include "utils.h"
#include <vector>

class Node {
public:
    Node(Board gameStatus, bool max) {
        this->gameStatus = gameStatus;
        this->max = max;
    }
    
    int evaluate() const;

private:
    Board gameStatus;
    bool max;
    std::vector<Node> childs;

    bool equals(const Pair& pair1, const Pair& pair2) const;
};

#endif