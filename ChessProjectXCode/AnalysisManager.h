#ifndef AnalysisManager_H
#define AnalysisManager_H

#include <fstream>
#include "ChessBoard.h"
#include "GameManager.h"
#include "RAFile.h"
#include <string>

class AnalysisManager {
private:
    RAFile<Move> input;
    GameManager gm;
public:
    AnalysisManager(std::string fileName);
    //Plays through the game given when this instance was created
    void play();
    //Checks that the AnalysisManager was created successfully
    operator bool() const;
    //Displays a menu for the user to choose from
    int displayMenu() const;
    //Displays the UI Update and input for the user, with the custom message above
    int displayUI(std::string str, bool whitesTurn);
};

#endif
