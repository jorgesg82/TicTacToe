#include "ia.h"

Node::Node(const Board &gameStatus, const bool &max)
{
    this->gameStatus = gameStatus;
    this->max = max;
}

bool isValid(Pair pair)
{
    if (pair.first < 0 || pair.first > 2 || pair.second < 0 || pair.second > 2)
    {
        return false;
    }
    return true;
}

bool Node::equals(const Pair &pair1, const Pair &pair2) const
{
    if (gameStatus.getValue(pair1) == gameStatus.getValue(pair2))
    {
        return true;
    }
    return false;
}

int Node::evaluate() const
{
    // Check if win
    int end = gameStatus.isWin();
    if (end != -1)
    {
        switch (end)
        {
        case 0:
            return 0; // Draw
            break;
        case 1:
            return -1; // Player wins
            break;
        case 2:
            return 1; // IA wins
            break;
        }
    }
    // Branch node
    // Create and evaluate childs
    int bestOption;
    if (this->max)
        bestOption = -2;
    else
        bestOption = 2;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            box b = gameStatus.getValue(std::make_pair(i, j));
            if (b == box::VOID)
            {
                Board newStatus = gameStatus;
                if (this->max)
                {
                    newStatus.setValue(std::make_pair(i, j), box::O);
                }
                else
                {
                    newStatus.setValue(std::make_pair(i, j), box::X);
                }

                Node child = Node(newStatus, !this->max);
                int value = child.evaluate();

                if (this->max)
                {
                    if (value > bestOption)
                        bestOption = value;
                    //if (value == 1)
                        //return 1;
                }
                else
                {
                    if (value < bestOption)
                        bestOption = value;
                    //if (value == -1)
                        //return -1;
                }
            }
        }
    }
    return bestOption;
}

Pair IA::play(const Board &board) const
{
    Pair choice;
    Board playBoard;
    int bestOption = -2;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (board.getValue(std::make_pair(i, j)) != box::VOID)
                continue;
            playBoard = board;
            playBoard.setValue(std::make_pair(i, j), box::O);
            Pair currentChoice(i, j);
            Node node = Node(playBoard, false);
            int value = node.evaluate();
            if (value > bestOption)
            {
                bestOption = value;
                choice = currentChoice;
            }
        }
    }
    return choice;
}