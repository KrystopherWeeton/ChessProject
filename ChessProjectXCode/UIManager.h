#ifndef UIManager_H
#define UIManager_H

#include "Move.h"
#include <fstream>

#include <vector>

class UIManager {
private:
public:
    //Manages the menu for the user
    void menu();
    //Displays all options for the user
    void displayOptions();
    //Gets an option from the user
    bool getOption(int& i);
    //Processes an option from the user
    void processOption(int i);
    //Loads a game from the file menu
    std::string chooseFile();
    //Saves a game to a text file
    void saveGame(std::string name, std::vector<Move>);
};

#endif
