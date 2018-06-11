#include "GameManager.h"
#include <iostream>
#include <vector>
#include <string>
#include "globalFunctions.h"

#define BOLDBLACK "\033[1m\033[30m"
#define RESET "\033[0m"

std::vector<Move> GameManager::play() {
    
    // A list of all the moves, to allow for saving the game to a random-access file
    std::vector<Move> moves;
    // Sets it initially to blacks turn, as it is about to switch
    bool whitesTurn = false;
    do {                                            //Game Loop
        
        // Updates the pre-conditions for the loop before operating
        whitesTurn = !whitesTurn;
        
        // Displays the board to the user, before gathering input
        globalFunctions::clearConsole();
        std::cout << (whitesTurn ? "White's" : "Black's") << " turn to play." << std::endl;
        board.print(whitesTurn, std::cout);
        std::cout << std::endl;
        
        // Processes the turn for the player
        Move m = turn(whitesTurn);
        if (m == EXIT)
            return std::vector<Move>();
        moves.push_back(m);
        
        // Checks for victory, and repeats if there has been no victory
    } while (!victory(whitesTurn));
    
    // The remainder of the function performs basic I/O for saving the game
    
    board.print(whitesTurn, std::cout);
    std::cout << BOLDBLACK << (whitesTurn ? "White" : "Black") << " has won the game." << RESET << std::endl;
    std::cin.get();      //Pauses the console to allow for seeing the checkmate
    
    //Checks with the user to see if they would like to save the game
    char c;
    globalFunctions::clearConsole();
    std::cout << "Would you like to save this game to your saved games? (y/n)" << std::endl;
    std::cin.get(c);
    
    //Returns the game if it should be saved, and no moves if it shouldn't
    return (c == 'y' || c == 'Y') ? moves : std::vector<Move>();
}

std::string GameManager::promptMove() {
    std::string s;
    std::cout << "Enter your desired move[ IdentifierColRow ]. Enter 'exit' to exit the program" << std::endl;
    std::cout << "Enter 'Castle K / Q' to castle your king on your King's / Queen's side." << std::endl;
    std::cout << BlinkingText << '>' << resetText;
    getline(std::cin, s);
    return s;
}

DecodeReturn GameManager::decodeMove(bool whiteTurn, std::string moveInput, Move& move) {
    // Checks if we must deal with one of the non-standard moves
    if (moveInput.size() != 3) {
        if ( moveInput.size() >= 8 &&
            ( moveInput.substr(0, 6)== "Castle" || moveInput.substr(0, 6) == "castle") ) {   // Castling
            
            bool king = (tolower(moveInput.at(7)) == 'k');
            bool canCastle = board.canCastle(whiteTurn, king);
            
            if (canCastle) {
                move = (king) ? KING_CASTLE : QUEEN_CASTLE;
                return DecodeReturn::Success;
            } else
                return DecodeReturn::ImpossibleMove;
        
        }
        return IllegalInput;
    }
    
    char iden = std::toupper(moveInput.at(0));
    int x = std::toupper(moveInput.at(1)) - 'A';    // 0 - 7
    int y = moveInput.at(2) - '1';    //0 - 7
    
    //Checks validity of the results
    if (iden != 'P' && iden != 'R' && iden != 'N' && iden != 'B' && iden != 'Q' && iden != 'K')
        return IllegalInput;
    if (x < 0 || x > 7 || y < 0 || y > 7)
        return IllegalInput;
    
    
    std::vector<Location> locations = board.gatherFromLocations(x, y, iden, whiteTurn);
    
    //Cleanses the locations vector, to only include the pieces which can actually reach the location
    for (auto it = locations.begin(); it != locations.end(); it++) {
        //Creates a location for us to manage easier
        Location l = *it;
        
        //Checks if the location is a valid one upon the board
        if (l.x < 0 || l.x > 7 || l.y < 0 || l.y > 7) {
            locations.erase(it);
            it--;
        } else {
            
            //Split into two separate if statements for readability
            
            //Checks if the piece we are looking for is at the location
            char c;
            if (board.idenAt(l, c) != true || c != iden || board.isWhite(l) != whiteTurn) {
                locations.erase(it);
                it--;
            }
        }
    }
    
    //Note: At this point, locations contains only those possibilites which could be used
    
    if (locations.size() > 1) { //Ambigious Choice
        std::cout << "Ambigious move choice. Choose one of the following squares to move from. " << std::endl;
        int k;
        for (int i = 0; i < locations.size(); i++) {
            std::cout << i + 1 << ": " << locations[i] << std::endl;
        }
        std::cin >> k;
        std::cin.ignore();
        while (k < 1 || k > locations.size()) {
            std::cout << "You must pick one of the above choices." << std::endl;
            std::cin >> k;
        }
        
        Location l = locations[k - 1];
        locations = std::vector<Location>();
        locations.push_back(l);
    }
    
    //Checks for a move with no locations. i.e. a move which can not be done.
    if (locations.size() == 0) {
        return ImpossibleMove;
    }
    
    move = Move(locations[0], Location(x, y));
    return Success;
}

Move GameManager::turn(bool whitesTurn) {
    Legality isLegal;
    Move m(0, 0);
    do {
        
        std::string s;
        DecodeReturn d;
        
        do {
            s = promptMove();
            if (s == "exit")
                return EXIT;
            d = decodeMove(whitesTurn, s, m);
            switch (d) {
                case DecodeReturn::IllegalInput: {
                    std::cout << "Illegal move input. Please try again." << std::endl;
                    break;
                }
                case DecodeReturn::ImpossibleMove: {
                    std::cout << "You have no pieces which can make that move. Please try again." << std::endl;
                    break;
                }
            }
        } while (d != DecodeReturn::Success);
        
        //Checks legality of the move, and then does the move if it is legal
        isLegal = board.doMove(whitesTurn, m, (whitesTurn ? whitePoints : blackPoints));
        
        if (isLegal == Legality::KingInCheck)
            std::cout << "You can not move there while your king is in check" << std::endl;
        else if (isLegal == Legality::PutsKingInCheck)
            std::cout << "You can not mvoe there because it puts your king in check" << std::endl;
        else if (isLegal == Legality::CantCastle)
            std::cout << "You can not castle at this time" << std::endl;
        
    } while (isLegal != Legality::Legal);
    
    return m;
}

void GameManager::newGame() {
    std::cerr << "This has not been implemented, as it is not necessary" << std::endl;
}

bool GameManager::victory(bool whitesTurn) {
    //Checks if the king is in check
    if (!board.kingInCheck(!whitesTurn))
        return false;
    
    //Gather all legal moves
    std::vector<Move> legalMoves = board.gatherAllLegalMoves(!whitesTurn);
    
    auto inCheck = [this, whitesTurn] () -> bool {
        return board.kingInCheck(whitesTurn);
    };
    
    //Test them, checking to see if any of them protect the king
    for (auto it = legalMoves.cbegin(); it != legalMoves.cend(); it++) {
        Move m = *it;
        if (!board.testMove(*it, inCheck))
            return false;
    }
    
    return true;
}








