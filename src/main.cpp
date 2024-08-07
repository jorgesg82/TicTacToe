#include <iostream>  // For input and output
#include <string>    // For string usage
#include <termios.h> // For terminal configuration
#include <unistd.h>  // For file descriptors constants

#include "game.h"

using namespace std;

int selectOption(const string &header, const vector<string> &options)
{
    int selection = 0;

    auto showMenu = [&header, &options, &selection]() -> void
    {
        system("clear");
        cout << header + "\n";
        for (int i = 0; i < options.size(); ++i)
        {
            if (i == selection)
                cout << "> ";
            cout << options[i] + "\n";
        }
    };

    struct termios originalTerm,
        gameTerm;
    tcgetattr(STDIN_FILENO, &originalTerm); // Store original terminal configuration
    gameTerm = originalTerm;
    gameTerm.c_lflag &= ~ICANON;                 // Disable canon mode
    gameTerm.c_lflag &= ~ECHO;                   // Disable echo mode
    tcsetattr(STDIN_FILENO, TCSANOW, &gameTerm); // Change terminal configuration

    bool selected = false;
    char key;

    showMenu();
    while (!selected)
    {
        key = getchar();
        if (key == 10) // ENTER
        {
            selected = true;
            break;
        }
        if (key != 27) // ESC
            continue;
        key = getchar();
        if (key != '[')
            continue;
        key = getchar();
        switch (key)
        {
        case 'A': // UP
            if (selection > 0)
                --selection;
            showMenu();
            break;
        case 'B': // DOWN
            if (selection < options.size() - 1)
                ++selection;
            showMenu();
            break;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm); // Restore terminal configuration

    return selection;
}

int main()
{
    bool endGame = false;
    system("clear");
    while (!endGame)
    {
        bool ia;
        bool selected = false;
        vector<string> options;
        string header;

        options = {"Player", "Machine"};
        header = "Do you want to play against another player or against the machine?\n";
        switch (selectOption(header, options))
        {
        case 0:
            ia = false;
            break;
        case 1:
            ia = true;
            break;
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
                header = "OOPS, it seems that you have reached a draw\n";
                break;
            case 1:
                winner = &player1;
                header = "CONGRATULATIONS " + *winner + " !, you have won\n";
                break;
            case 2:
                header = "OH NO! IA has won\n";
                break;
            }
        }
        else
        {
            switch (gameStatus)
            {
            case 0:
                header = "OOPS, it seems that you have reached a draw\n";
                break;
            default:
                if (gameStatus == 1)
                    winner = &player1;
                else
                    winner = &player2;
                header = "CONGRATULATIONS " + *winner + " !, you have won\n";
                break;
            }
        }

        options = {"Yes", "Please don't, I can't stand it anymore"};
        header = header + "Do you want to play again?";
        switch (selectOption(header, options))
        {
        case 1:
            endGame = true;
            break;
        }
        delete (game);
        tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm); // Restore terminal configuration
    }

    return 0;
}