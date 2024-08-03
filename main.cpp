#include <iostream>  // For input and output
#include <string>    // For string usage
#include <termios.h> // For terminal configuration
#include <unistd.h>  // For file descriptors constants

#include "game.h"

using namespace std;

int main()
{
    bool endGame = false;
    system("clear");
    while (!endGame)
    {
        bool ia;
        bool selected = false;
        while (!selected)
        {
            cout << "Do you want to play against another player or against the machine?\n";
            cout << "1. Player\n2. Machine\n";
            cout << "Type a number: ";
            char selection;
            cin >> selection;
            switch (selection)
            {
            case '1':
                selected = true;
                ia = false;
                break;
            case '2':
                selected = true;
                ia = true;
                break;
            default:
                system("clear");
                cout << "\n\nType a valid number!\n\n";
            }
        }

        string player1, player2;
        // TODO Check if names are valid
        system("clear");
        if (!ia)
        {
            cout << "Name of the first player: ";
            cin >> player1;
            system("clear");
            cout << "Name of the second player: ";
            cin >> player2;
            system("clear");
        }
        else
        {
            cout << "Name of the player: ";
            cin >> player1;
            system("clear");
        }
        cin.ignore(); // To erase final intro

        struct termios originalTerm, gameTerm;
        tcgetattr(STDIN_FILENO, &originalTerm); // Store original terminal configuration
        gameTerm = originalTerm;
        gameTerm.c_lflag &= ~ICANON;                 // Disable canon mode
        gameTerm.c_lflag &= ~ECHO;                   // Disable echo mode
        tcsetattr(STDIN_FILENO, TCSANOW, &gameTerm); // Change terminal configuration

        Game *game;
        if (ia)
            game = new Game(player1);
        else
            game = new Game(player1, player2);

        // Start game!
        int gameStatus = game->startGame();

        // Show result
        string *winner;
        if (ia)
        {
            switch (gameStatus)
            {
            case 0:
                cout << "OOPS, it seems that you have reached a draw";
                break;
            case 1:
                winner = &player1;
                cout << "CONGRATULATIONS " << *winner << " !, you have won";
                break;
            case 2:
                cout << "OH NO! IA has won";
                break;
            }
        }
        else
        {
            switch (gameStatus)
            {
            case 0:
                cout << "OOPS, it seems that you have reached a draw";
                break;
            default:
                if (gameStatus == 1)
                    winner = &player1;
                else
                    winner = &player2;
                cout << "CONGRATULATIONS! Player " << *winner << " has won";
                break;
            }
        }

        cout << "\n\nDo you want to play again? (Y/N): ";
        char key = getchar();
        if (key != 'Y' && key != 'y')
            endGame = true;

        tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm); // Restore terminal configuration
        system("clear");
        delete (game);
    }

    return 0;
}