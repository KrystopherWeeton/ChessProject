#include "UIManager.h"
#include "globalFunctions.h"
#include "GameManager.h"
#include "AnalysisManager.h"
#include "RAFile.h"

#include <vector>
#include <iostream>

#define BLINKINGTEXT "\033[5m"
#define RESETTEXT "\033[0m"


int UIManager::maxChoice = 4;

/*
    The function which displays the menu to the user on the primary text output.
 
*/
void UIManager::menu() {
    int x;
    
    while (true) {
        
        globalFunctions::clearConsole();    // Clears the console
        displayOptions();
        bool validChoice = getOption(x);
    
        while (!validChoice) {              // Forces the user to pick a valid choice
            std::cout << "You have not picked a valid option. Enter another." << std::endl;
            validChoice = getOption(x);
        }
        processOption(x);
        
    }
}

/*
 Displays all options to the user.
*/
void UIManager::displayOptions() {
    std::cout << "Chooose from one of the options below." << std::endl;
    std::cout << "(1) Play a game of Chess against another player" << std::endl;
    std::cout << "(2) Load a game of Chess from a file" << std::endl;
    std::cout << "(3) Convert a game to a text file" << std::endl;
    std::cout << "(4) Exit" << std::endl;
}

/*
 Gathers an option from the user
*/
bool UIManager::getOption(int& i) {
    std::cout << BLINKINGTEXT << '>';
    std::cout << RESETTEXT;
    std::cin >> i;
    std::cin.clear();                // Needed to clear failbit if input is not a character
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Needed to ignore subsequent characters of invalid input
    return !(std::cin.fail() || i < 1 || i > maxChoice);
}

/*
 Processes the option the user chose, taking the appropriate course of action
 */
void UIManager::processOption( int i ) {
    switch (i) {
        case 1: {   // Play a game
            GameManager gm;
            std::vector<Move> game = gm.play();
            
            // Saves the game if the user requests it
            if (game.size() != 0) {             // If the user decided to save the game
                globalFunctions::clearConsole();
                std::cout << "Enter the name you would like to use for the game" << std::endl;
                std::string name = globalFunctions::getInput();
                saveGame(name, game);
            }
            break;
        }
        case 2: {   // Open a file for analysis
            std::cout << "Enter the path and file name you would like to open. (if it is in the default location,you need only enter the name of the file" << std::endl;
            std::string path = chooseFile();
            AnalysisManager am(path);
            am.play();
            break;
        }
        case 3: {   // Converting a game to a text file
            std::cout << "Enter the path for the game you would like to convert. Enter just the name, if it is in the default directory." << std::endl;
            std::string path = chooseFile();
            std::cout << "Enter the directory you would like to save the file to. Enter default for default directory." << std::endl;
            std::string saveDir = chooseFile();
            if (saveDir == "default")
                saveDir = "";
            RAFile<Move> file;
            file.loadFile(path);
            if (!file.isOpen()) {
                std::cout << "There was an error opening the file. Try again." << std::endl;
            }
            
            bool success = globalFunctions::createGameFile(file, path);
            
            if (!success) {
                std::cout << "There was an error in creating the text file. Please try again." << std::endl;
            }
            
            break;
        }
        case 4: {   // Exit
            exit(0);    //Games are automatically saved when GameStorage is deleted, so no worries
            break;
        }
        default: {
            std::cerr << "Who messed up, and allowed input that isn't valid. Kill them." << std::endl;
        }
    }
}

/*
 Prompts the user for a line of text, and then inputs it
 */
std::string UIManager::chooseFile() {
    std::string str;
    getline(std::cin, str);
    return str;
}

/*
 Goes through the process of saving the game.
 NOTE: Might want to move this functionality to a different class, but for now it is fine.
 */
void UIManager::saveGame(std::string name, std::vector<Move> game) {
    char c;
    std::cout << RESETTEXT << "Would you like to save in the default location? (y/n)" << std::endl;
    std::cin >> c;
    std::cin.ignore();
    std::string path = name;
    if (c != 'y' && c != 'Y') { // Different location
        std::cout << "Enter the path to the location you would like to save the file to" << std::endl;
        path = chooseFile();
        path.append("/");
        path.append(name);
    }
    
    //Makes the file
    RAFile<Move> file;
    file.newfile(path);
    if (!file.isOpen()) {
        std::cerr << "File to save game could not be created" << std::endl;
        return;
    }
    
    //Fills the file
    for (auto it = game.begin(); it != game.end(); it++) {
        file.append(*it);
    }

}








