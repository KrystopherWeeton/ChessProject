#ifndef GameManager_H
#define GameManager_H

#include "ChessBoard.h"
#include "Move.h"
#include <string>
#include <vector>
#include "DecodeReturn.h"

#define BlinkingText "\033[5m"
#define resetText "\033[0m"

class GameManager {
private:
    ChessBoard board;
    
    int whitePoints = 0;
    int blackPoints = 0;
    
public:
    
    friend class AnalysisManager;
    
    //Plays a game
    std::vector<Move> play();
    
    //Prompts for a move from the user, and returns the value
    std::string promptMove();
    
    //Decodes a move, returing a value representing whether the move is legal and why it isn't legal
    DecodeReturn decodeMove(bool whiteTurn, std::string, Move&);
    
    //Processes a turn for one of the players
    Move turn(bool whitesTurn);
    
    //Resets the game for a new game
    void newGame();
    
    //Checks if a player has won the game, returning the appropriate result
    bool victory(bool whitesTurn);
};

#endif
