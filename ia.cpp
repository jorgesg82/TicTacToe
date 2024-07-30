#include "ia.h"

bool isValid(Pair pair) {
    if (pair.first < 0 || pair.first > 2
    || pair.second < 0 || pair.second > 2) {
        return false;
    }
    return true;
}

bool Node::equals(const Pair& pair1, const Pair& pair2) const {
    if (this->gameStatus[pair1.first][pair1.second] == this->gameStatus[pair2.first][pair2.second]) {
        return true;
    }
    return false;
}

int Node::evaluate() const {

    // Branch node
    // Create and evaluate childs
    bool isChild = true;
    int bestOption;
    int rowIndex = 0;
    for (const auto& row : this->gameStatus) {
        int colIndex = 0;
        for (const box& b : row) {
            if (b == box::VOID) {
                isChild = false;
                if (this->max) bestOption = -1;
                else bestOption = 1;
                Board newStatus = gameStatus;
                if (this->max) newStatus[rowIndex][colIndex] = box::X;
                else newStatus[rowIndex][colIndex] = box::O;
                Node child = Node(newStatus, !this->max);
                int value = child.evaluate();
                if (this->max) {
                    if (value == 1) return 1;
                    if (value > bestOption) bestOption = value;
                } else {
                    if (value == -1) return -1;
                    if (value < bestOption) bestOption = value;
                }
            }
            ++colIndex;
        }
        ++rowIndex;
    }
    if (!isChild) return bestOption;

    // Leaf node
    bool win = false;
    bool winner;

    for (int a = 0; a < 3; ++a) {
        for (int b = 0; b < 3; ++b) {
            // Go through all the boxes
            Pair currentPosition = std::make_pair(a,b);
            for (int i = -1; i < 2; ++i) {
                for (int j = -1; j < 2; ++j) {
                    // Go through all adjacent boxes
                    if (i == 0 && j == 0) continue; // Avoid current box 
                    Pair secondPosition = std::make_pair(a+i, b+j);
                    if (!isValid(secondPosition)) continue; // Out of bounds
                    if (equals(currentPosition, secondPosition)) {
                        // Second position is valid. Check third position
                        std::array<Pair, 2> positionsToCheck = {
                            std::make_pair(a+2*i,b+2*j),
                            std::make_pair(a-i,b-j)
                        };
                        for (const Pair &thirdPosition : positionsToCheck) {
                            if (isValid(thirdPosition) && equals(currentPosition, thirdPosition)) {
                                win = true;
                                box value = gameStatus[currentPosition.first][currentPosition.second];
                                if(value == box::X) return 1; // Player1 wins
                                return -1; // Player2 wins
                                break;
                            }
                        }
                    }
                }
            }
        }   
    }
    return 0; // Reached a draw
}