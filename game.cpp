#include "game.h"
#include "utils.h"

#include <thread>   // For threads manage
#include <mutex>    // For concurrency control
#include <chrono>   // For time manage
#include <iostream> // For input and output

using namespace std;

const array<array<Pair, 10>, 4> Game::directions{{
    {Pair(-1, 0), Pair(-2, 0), Pair(-1, -1), Pair(-2, -1),
     Pair(-1, 1), Pair(-2, 1), Pair(-1, -2), Pair(-2, -2),
     Pair(-1, 2), Pair(-2, 2)}, // UP
    {Pair(1, 0), Pair(2, 0), Pair(1, -1), Pair(2, -1),
     Pair(1, 1), Pair(2, 1), Pair(1, -2), Pair(2, -2),
     Pair(1, 2), Pair(2, 2)}, // DOWN
    {Pair(0, -1), Pair(0, -2), Pair(-1, -1), Pair(-1, -2),
     Pair(1, -1), Pair(1, -2), Pair(-2, -1), Pair(-2, -2),
     Pair(2, -1), Pair(2, -2)}, // LEFT
    {Pair(0, 1), Pair(0, 2), Pair(-1, 1), Pair(-1, 2),
     Pair(1, 1), Pair(1, 2), Pair(-2, 1), Pair(-2, 2),
     Pair(2, 1), Pair(2, 2)} // RIGHT
}}; // All posible directions when you press an arrow to move. There are already in priority order

Game::Game(const string &player1)
{
    gameBoard = Board();
    tempBoard = Board();

    turn = false;
    turnCount = 0;
    iaPlayer = true;
    end = false;

    this->player1 = player1;
    ia = IA();

    tempPos = make_pair(0, 0);
}

Game::Game(const string &player1, const string &player2)
{
    gameBoard = Board();
    tempBoard = Board();

    turn = false;
    turnCount = 0;
    iaPlayer = false;
    end = false;

    this->player1 = player1;
    this->player2 = player2;

    tempPos = make_pair(0, 0);
}

void Game::changePos(const direction &dir)
{
    Pair nextPos;
    for (auto i = directions[dir].begin(); i != directions[dir].end(); ++i)
    { // Check all possible directions
        // Calculate new position
        nextPos = Pair(tempPos.first + i->first, tempPos.second + i->second);

        // Check if new position is valid
        if (nextPos.first < 0 || nextPos.second < 0 || nextPos.first > 2 || nextPos.second > 2)
        {
            continue;
        } // Out of board bounds
        if (gameBoard.getValue(nextPos) != box::VOID)
            continue; // Position already occupied

        // Change temporal position in temporal board
        mtxTempBoard.lock();
        tempBoard.setValue(tempPos, box::VOID);
        if (turn == 0)
            tempBoard.setValue(nextPos, box::X);
        else
            tempBoard.setValue(nextPos, box::O);
        mtxTempBoard.unlock();
        tempPos = nextPos;
        break;
    }
}

void Game::drawBoard(const Board &board) const
{
    if (turn == 0)
    {
        cout << "Now is " + player1 + " time!\n";
    }
    else
    {
        cout << "Now is " + player2.value_or("IA") + " time!\n";
    }
    cout << "-------------\n";
    for (int i = 0; i < 3; ++i)
    {
        cout << "| ";
        for (int j = 0; j < 3; ++j)
        {
            switch (board.getValue(make_pair(i, j)))
            {
            case box::O:
                cout << "O";
                break;
            case box::X:
                cout << "X";
                break;
            case box::VOID:
                cout << " ";
                break;
            }
            cout << " | ";
        }
        cout << "\n";
        cout << "-------------\n";
    }
}

void Game::drawGame()
{
    while (!end && turnCount < 9)
    {
        system("clear");
        mtxGameBoard.lock();
        drawBoard(gameBoard);
        mtxGameBoard.unlock();
        this_thread::sleep_for(chrono::milliseconds(300));
        system("clear");
        mtxTempBoard.lock();
        drawBoard(tempBoard);
        mtxTempBoard.unlock();
        this_thread::sleep_for(chrono::milliseconds(300));
    }
    return;
}

int Game::startGame()
{
    // Start to draw board
    drawer = thread(&Game::drawGame, this);

    while (!end && turnCount < 9)
    {
        // Reach first void position
        bool reached = false;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; j++)
            {
                if (gameBoard.getValue(make_pair(i, j)) == box::VOID)
                {
                    tempPos.first = i;
                    tempPos.second = j;
                    reached = true;
                    break;
                }
            }
            if (reached)
                break;
        }
        mtxTempBoard.lock();
        if (turn == 0)
            tempBoard.setValue(tempPos, box::X);
        else
            tempBoard.setValue(tempPos, box::O);
        mtxTempBoard.unlock();

        if (turn && iaPlayer)
        {
            // IA turn
            tempPos = ia.play(gameBoard);
        }
        else
        {
            // Player turn

            // Wait to the player for press any key
            bool choosed = false;
            while (!choosed)
            {
                char key;
                bool arrowPressed = false;
                while (!arrowPressed)
                {
                    key = getchar();
                    if (key == 10)
                    {
                        choosed = true;
                        break;
                    }
                    if (key != 27)
                        continue; // ESC
                    key = getchar();
                    if (key != '[')
                        continue;
                    key = getchar();
                    switch (key)
                    {
                    case 'A': // UP
                        changePos(direction::UP);
                        arrowPressed = true;
                        break;
                    case 'B': // DOWN
                        changePos(direction::DOWN);
                        arrowPressed = true;
                        break;
                    case 'C': // RIGHT
                        changePos(direction::RIGHT);
                        arrowPressed = true;
                        break;
                    case 'D': // LEFT
                        changePos(direction::LEFT);
                        arrowPressed = true;
                        break;
                    }
                }
            }
        }

        // Change board
        mtxGameBoard.lock();
        mtxTempBoard.lock();
        if (turn == 0)
        {
            gameBoard.setValue(tempPos, box::X);
            tempBoard.setValue(tempPos, box::X);
        }
        else
        {
            gameBoard.setValue(tempPos, box::O);
            tempBoard.setValue(tempPos, box::O);
        }
        mtxGameBoard.unlock();
        mtxTempBoard.unlock();

        // Check if end has been reached
        if (gameBoard.isWin() >= 0)
        {
            end = true;
            break;
        }

        // Turn finished
        turn = !turn;
        ++turnCount;
    }

    // Game has ended.
    // Waiting drawer to stop
    drawer.join();
    system("clear");
    drawBoard(gameBoard);

    return gameBoard.isWin();
}
