#ifndef globalFunctions_hpp
#define globalFunctions_hpp

#include <string>
#include <fstream>
#include "ChessBoard.h"
#include "RAFile.h"
#include "Move.h"

class globalFunctions {
private:
    
    static std::string createGameEntry(RAFile<Move>&, ChessBoard, bool, int, Move&);
    
public:
    static void clearConsole();
    
    static std::string getInput();
    
    static bool createGameFile(RAFile<Move>&, std::string);
    
};

#endif
