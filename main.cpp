#include <iostream> // For input and output
#include <string> // For string usage
#include <utility> // For pair usage
#include <array> // For C++11 array usage
#include <thread> // For threads manage
#include <mutex> // For concurrency control
#include <chrono> // For time manage
#include <termios.h> // For terminal configuration
#include <unistd.h> // For file descriptors constants

using namespace std;

string player1;
string player2;
bool turn; // A value of '0' or 'false' means that is player1 turn, a value of '1' or 'true' means is player2 turn.
bool win;
mutex mtxBoard;
mutex mtxTempBoard;
const array<array<pair<int, int>, 10>, 4> directions {{
        {pair<int, int>(-1, 0), pair<int, int>(-2, 0), pair<int, int>(-1, -1), pair<int, int>(-2, -1),
         pair<int, int>(-1, 1), pair<int, int>(-2, 1), pair<int, int>(-1, -2), pair<int, int>(-2, -2),
         pair<int, int>(-1, 2), pair<int, int>(-2, 2)}, // UP
        {pair<int, int>(1, 0), pair<int, int>(2, 0), pair<int, int>(1, -1), pair<int, int>(2, -1),
         pair<int, int>(1, 1), pair<int, int>(2, 1), pair<int, int>(1, -2), pair<int, int>(2, -2),
         pair<int, int>(1, 2), pair<int, int>(2, 2)}, // DOWN
        {pair<int, int>(0, -1), pair<int, int>(0, -2), pair<int, int>(-1, -1), pair<int, int>(-1, -2),
         pair<int, int>(1, -1), pair<int, int>(1, -2), pair<int, int>(-2, -1), pair<int, int>(-2, -2),
         pair<int, int>(2, -1), pair<int, int>(2, -2)}, // LEFT
        {pair<int, int>(0, 1), pair<int, int>(0, 2), pair<int, int>(-1, 1), pair<int, int>(-1, 2),
         pair<int, int>(1, 1), pair<int, int>(1, 2), pair<int, int>(-2, 1), pair<int, int>(-2, 2),
         pair<int, int>(2, 1), pair<int, int>(2, 2)}  // RIGHT
    }}; // All posible directions when you press an arrow to move. There are already in priority order

enum class box {
    X,
    O,
    VOID
};
enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

array<array<box, 3>, 3> board; // Multidimensional array 3x3
array<array<box, 3>, 3> tempBoard;

void startGame() {
    pair<int, int> tempPos(0,0);
    bool reached;
    int count = 0; // Count number of turns
    
    thread drawer([&count] {

        // Draw function
        auto draw = [](const array<array<box, 3>, 3> board) -> void {
            if (turn == 0) {
                cout << "Now is " + player1 + " time!\n";
            } else {
                cout << "Now is " + player2 + " time!\n";
            }
            cout << "-------------\n";
            for (int i = 0; i < 3; ++i) {
                cout << "| ";
                for (int j = 0; j < 3; ++j) {
                    switch (board[i][j]) {
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
        };

        while (!win && count < 9) {
            system("clear");
            mtxBoard.lock();
            draw(board);
            mtxBoard.unlock();
            this_thread::sleep_for(chrono::milliseconds(300));
            system("clear");
            mtxTempBoard.lock();
            draw(tempBoard);
            mtxTempBoard.unlock();
            this_thread::sleep_for(chrono::milliseconds(300));
        }
        return;
    });

    // Lambda function that, given a direction, calculates where should be the next position and changes to it
    auto changePos = [&tempPos](const direction dir) -> void {
        pair<int, int> nextPos;
        for(auto i = directions[dir].begin(); i != directions[dir].end(); ++i) { // Check all possible directions
            // Calculate new position
            nextPos = pair<int,int>(tempPos.first + i->first, tempPos.second + i->second);

            // Check if new position is valid
            if (nextPos.first < 0 
                || nextPos.second < 0
                || nextPos.first > 2
                || nextPos.second > 2)
            {
                continue;
            } // Out of board bounds
            if (board[nextPos.first][nextPos.second] != box::VOID) continue; // Position already occupied
        
            // Change temporal position in temporal board
            mtxTempBoard.lock();
            tempBoard[tempPos.first][tempPos.second] = box::VOID;
            if(turn == 0) tempBoard[nextPos.first][nextPos.second] = box::X;
            else tempBoard[nextPos.first][nextPos.second] = box::O;
            mtxTempBoard.unlock();
            tempPos = nextPos;
            break;
        }
    };

    while(!win && count < 9) {
        // Change turn
        turn = !turn;

        // Reach first void position
        reached = false;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; j++) {
                if(board[i][j] == box::VOID) {
                    tempPos.first = i;
                    tempPos.second = j;
                    reached = true;
                    break;
                }
            }
            if (reached) break;
        }
        mtxTempBoard.lock();
        if(turn == 0) tempBoard[tempPos.first][tempPos.second] = box::X;
        else tempBoard[tempPos.first][tempPos.second] = box::O;
        mtxTempBoard.unlock();

        // Wait to the player for press any key
        bool choosed = false;
        while (!choosed) {
            char key;
            bool arrowPressed = false;
            while (!arrowPressed) {
                key = getchar();
                if (key == 10) {
                    choosed = true;
                    break;
                }
                if (key != 27) continue; // ESC
                key = getchar();
                if (key != '[') continue;
                key = getchar();
                switch (key) {
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

        ++count;

        // Change board
        mtxBoard.lock();
        if(turn == 0) board[tempPos.first][tempPos.second] = box::X;
        else board[tempPos.first][tempPos.second] = box::O;
        mtxBoard.unlock();

        // Check if win has been reached
        
        // Check nearby boxes
        pair<int,int> secondPos(0,0);
        pair<int,int> thirdPos(0,0);
        for (int i = -1; i < 2; ++i) {
            secondPos.first = tempPos.first + i;
            if (secondPos.first < 0 || secondPos.first > 2) continue;
            for (int j = -1; j < 2; ++j) {
                secondPos.second = tempPos.second + j;
                if (secondPos == tempPos || secondPos.second < 0 || secondPos.second > 2) continue;
                if(turn == 0 && board[secondPos.first][secondPos.second] == box::X
                    || turn == 1 && board[secondPos.first][secondPos.second] == box::O)
                {
                    thirdPos.first = secondPos.first + i;
                    thirdPos.second = secondPos.second + j;
                    if (thirdPos.first >= 0 && thirdPos.first <= 2) {
                        if (thirdPos.second >= 0 && thirdPos.second <= 2) {
                            if(turn == 0 && board[thirdPos.first][thirdPos.second] == box::X
                                || turn == 1 && board[thirdPos.first][thirdPos.second] == box::O)
                            {
                                win = true;
                                break;
                            }
                        }
                    }

                    thirdPos.first = tempPos.first - i;
                    thirdPos.second = tempPos.second - j;
                    if (thirdPos.first >= 0 && thirdPos.first <= 2) {
                        if (thirdPos.second >= 0 && thirdPos.second <= 2) {
                            if(turn == 0 && board[thirdPos.first][thirdPos.second] == box::X
                                || turn == 1 && board[thirdPos.first][thirdPos.second] == box::O)
                            {
                                win = true;
                                break;
                            }
                        }
                    }
                }
            }
           if (win) break;
        }
    }

    // Win has been reached. Waiting drawer to stop
    drawer.join();
    return;
}

int main () {
    turn = 1;

    system("clear");
    cout << "Let's play Tic Tac Toe!\n\n";

    // TODO Check if names are valid
    cout << "Name of the first player: ";
    cin >> player1;
    system("clear");
    cout << "Name of the second player: ";
    cin >> player2;
    cin.ignore(); // To erase final intro
    system("clear");

    struct termios originalTerm, gameTerm;
    tcgetattr(STDIN_FILENO, &originalTerm); // Store original terminal configuration
    gameTerm = originalTerm;
    gameTerm.c_lflag &= ~ICANON; // Disable canon mode
    gameTerm.c_lflag &= ~ECHO; // Disable echo mode
    tcsetattr(STDIN_FILENO, TCSANOW, &gameTerm); // Change terminal configuration

    bool endGame = false;
    while(!endGame) {
        // Init void board
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; j++) {
                board[i][j] = box::VOID;
            }
        }

        // Init void tempBoard
        tempBoard = board;

        win = false;

        // Start game!
        startGame();

        // Win has been reached
        string* winner;
        if (turn) {
            winner = &player2;
        } else {
            winner = &player1;
        }
        if (!win) {
            cout << "OOPS, it seems that you have reached a draw";
        } else {
            cout << "CONGRATULATIONS! Player " << *winner << " has won";
        }
        cout << "\n\nDo you want to play again? (Y/N): ";
        char key = getchar();
        if (key != 'Y') endGame = true;
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm); // Restore terminal configuration
    system("clear");

    return 0;
}