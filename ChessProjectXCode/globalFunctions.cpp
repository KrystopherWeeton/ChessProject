#include "globalFunctions.h"
#include "ChessBoard.h"
#include <iostream>

#define BLINKINGTEXT "\033[5m"
#define RESETTEXT "\033[0m"

void globalFunctions::clearConsole() {
    for (int i = 0; i < 30; i++)
        std::cout << std::endl;
}

std::string globalFunctions::getInput() {
    std::cout << BLINKINGTEXT << '>' << RESETTEXT;
    std::string s;
    std::cin >> s;
    return s;
}

bool globalFunctions::createGameFile(RAFile<Move>& file, std::string txtFileName) {
    
    // Checks if the file is valid or not
    if (!file.isOpen())
        return false;
    
    // Creates the text file for output
    std::ofstream output(txtFileName + ".txt");
    
    if (!output.is_open())
        return false;
    
    // Begins our iteration loop for the moves in the game
    int index = 1;
    int turnNumber = 1;
    Move m;
    ChessBoard board;
    board.reset();
    int garbage;
    bool whiteTurn = true;
    
    int y = file.size();
    
    while (index <= file.size()) {
        
        // Outputs the turn number
        output << turnNumber << ". ";
        
        // Determines the output for the white move
        std::string s = createGameEntry(file, board, whiteTurn, index, m);
        output << s;
        
        // Actually performs the move on the board
        board.doMove(whiteTurn, m, garbage);
        
        // Determines if we should be doing black's move
        if (index + 1 <= file.size()) {
            
            // Outputs blacks move
            output << " " << createGameEntry(file, board, !whiteTurn, index + 1, m);
            
            // Actually performs black's move on the board
            board.doMove(!whiteTurn, m, garbage);
            
            if (index + 2 <= file.size()) {
                output << " ";
            }
        }
        
        
        
        
        
        // Updates values for the next iteration
        turnNumber++;
        index += 2;
    }
    
    output << '#';
    
    return true;
}

std::string globalFunctions::createGameEntry(RAFile<Move>& file, ChessBoard board, bool whiteTurn, int index, Move& m) {

    // Gathers the move
    file.get(index, m);
    Location to = m.to;
    Location from = m.from;
    
    // Determines if we are castling
    if (m == KING_CASTLE || m == QUEEN_CASTLE) {
        return "0 - 0";
    }
    
    // Ensures that the move comes from a piece, and gathers the identifier
    Piece* p = board.at(m.from.x, m.from.y);
    if (p == nullptr)
        return "";
    
    // Gathers the basic string, that we can add to or remove from if necessary
    std::string moveString = (board.at(to) == nullptr) ? "" : "x";
    moveString.append(static_cast<std::string>(to));
    
    // Gathers the possible locations for the move to have come from
    std::vector<Location> locations = board.gatherFromLocations(to.x, to.y, p->getIdentifier(), whiteTurn);
    
    
    if (locations.size() > 1) { // We must specify further
        
        int sameXCount = 0;
        int sameYCount = 0;
        
        for (int i = 0; i < locations.size(); i++) {
            Location k = locations.at(i);
            if (k.x == from.x)
                sameXCount++;
            if (k.y == from.y)
                sameYCount++;
        }
        
        
        std::string str = "";
        
        if (sameXCount >= 1) {            // We need only specify the column
            str = {(char)('a' + from.x)};
        } else if (sameYCount >= 1) {     // We need only specify the row
            str = {(char)('1' + from.y)};
        } else {
            str = {(char)('a' + from.x), (char)('1' + from.y)};
        }
        
        moveString.insert(0, str);
    }
    
    // Attaches the identifier
    
    moveString.insert(0, 1, p->getIdentifier());
    
    return moveString;
}
