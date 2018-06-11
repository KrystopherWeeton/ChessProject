#include "AnalysisManager.h"
#include "globalFunctions.h"
#include <string>
#include <stack>

AnalysisManager::AnalysisManager(std::string fileName) {
    input.loadFile(fileName);
}

int AnalysisManager::displayUI(std::string str, bool whitesTurn) {
    globalFunctions::clearConsole();
    std::cout << str << std::endl;
    gm.board.print(true, std::cout);
    return displayMenu();
}

void AnalysisManager::play() {
    
    //Resets the board to starting position
    gm.board.reset();
    globalFunctions::clearConsole();
    
    //Tracker for when the user wants to leave, which will probably be replaced soon enough
    bool done = false;
    bool whitesTurn = false;
    int index = 0;
    std::stack<Piece*> taken;
    std::stack<int>     takenIndex;
    std::stack<int>     queenMakingIndeces;
    Move m;
    int choice = 3;
    
    
    while (!done) {
        
        /*
            Because the below code is fairly confusing, I find it necessary to list the pre-conditions and notes that govern the code below
            1. The Index, at this point in the code, is always the move which has been done last. This will be either (a) the last step forward, (b) the last move before the end, or (c) 0, indicating a starting position.
            2. The turn boolean represents the color which has moved last. i.e. if the last step was black, the color will be black. Thus, the boolean is initially set to be false (for black moved last), as white must move first
         
         
            Notes: 
         
                doMove() does check legality of the move, which is somewhat a waste and so it might be replaced by peroformMove
                the RAFile class startings indexing at 1 . . . size(). This was done as a test to see how it would work. I now realize that 0 . . . size() - 1, is actually easier for me to manage.
         
         */
        
        if (choice == 1) {                  // Step forward
            
            // checks if there are no more moves to perform
            if (index == input.size()) {
                choice = displayUI("You can not step any further", whitesTurn);
                continue;
            }
            
            // Increments counters to the proper locations for the next move
            index++;                        // Moves the index to the move about to be done
            whitesTurn = !whitesTurn;       // Moves the color to the move about to be done
            
            // Gathes the move details for the next move
            input.get(index, m);
            
            // Checks for pawn turning into a queen
            Piece* p = gm.board.at(m.from);
            if (p != nullptr && p->getIdentifier() == 'P' && ( (m.to.y == 7 && whitesTurn) || (m.to.y == 0 && !whitesTurn) ) ) {
                queenMakingIndeces.push(index);
            }
            
            // Updates our piece tracker, if necessary
            if (!gm.board.isEmpty(m.to)) {  // If we are taking a piece
                taken.push(gm.board.at(m.to));
                takenIndex.push(index);
            }
            
            // Performs the actual move, we pass in choice because we don't care about it's value anymore
            gm.board.doMove(whitesTurn, m, choice);
            
        } else if (choice == 2) {           // Step back
            
            // Note we are checking indeces before performing the move
            if (index == 0) {  // No prior moves
                choice = displayUI("You can not step back any further", whitesTurn);
                continue;
            }/* else if (index == 1) {    // Reset to initial position, may not be necessary
                gm.board.reset();
                taken = std::stack<Piece*>();
                takenIndex = std::stack<int>();
                whitesTurn = false;
                choice = displayUI("", whitesTurn);
                continue;
            }*/
            
            // Removes the previous move
            input.get(index, m);
            Piece* p = nullptr;
            if (!takenIndex.empty() && takenIndex.top() == index) {
                p = taken.top();
                p->activate();  // Activates the piece so it is drawn
                taken.pop();
                takenIndex.pop();
            }
            
            
            Move m2(m.to, m.from);
            
            gm.board.performMove(m2);
            
            gm.board.set(m2.from, p);
            
            // Checks if we are taking back a turn which made a pawn into a queen
            if (!queenMakingIndeces.empty() && queenMakingIndeces.top() == index) {
                
                // Gathers a pawn from the list of the piece set, so it is included in all functions
                Pawn* p = (whitesTurn ? gm.board.white : gm.board.black).findDeactivatedPawn();
                
                // Sets the location to the piece
                if ( p != nullptr) {    // Should always be able to run, but better safe
                    p->setLocation(m.from);     // Sets the location of the piece
                    p->activate();              // Activates the piece
                    Piece* queen = gm.board.at(m.from); // Gathers the queen so we can deactivate it
                    queen->deactivate();        // Deactivates the queen
                    gm.board.set(m.from, p);    // Set sthe location to the pawn
                }
                
                queenMakingIndeces.pop();
            }
            
            // Decrements values to the previous turn
            whitesTurn = !whitesTurn;
            index--;
            
        } else if (choice == 3) {           // Go to beginning
            
            // Reset the stacks
            takenIndex = std::stack<int>();
            taken = std::stack<Piece*>();
            queenMakingIndeces = std::stack<int>();
            
            // Resets the board
            whitesTurn = false;
            gm.board.reset();
            index = 0;
            
        } else if (choice == 4) {           // Go to end
            
            while (index < input.size()) {  // Loops through the game, without input
                input.get(++index, m);
                whitesTurn = !whitesTurn;
                if (!gm.board.isEmpty(m.to)) {  // If we are taking a piece
                    taken.push(gm.board.at(m.to));
                    takenIndex.push(index);
                }
                // Checks for pawn turning into a queen
                Piece* p = gm.board.at(m.from);
                if (p != nullptr && p->getIdentifier() == 'P' && ( (m.to.y == 7 && whitesTurn) || (m.to.y == 0 && !whitesTurn) ) ) {
                    queenMakingIndeces.push(index);
                }
                // Note: we pass in choice because we can corrupt it now
                gm.board.doMove(whitesTurn, m, choice);
            }
            
        } else if (choice == 5) {   // Exit Program
            
            done = true;
            
        }
        
        if (!done) {    //Gets input for the next operation
            choice = displayUI("", whitesTurn);
        }
    }
}

AnalysisManager::operator bool() const {
    return input.isOpen();
}

int AnalysisManager::displayMenu() const {
    std::cout << "Choose from one of the following options" << std::endl;
    std::cout << "(1) step forward" << std::endl;
    std::cout << "(2) step backward" << std::endl;
    std::cout << "(3) Return to turn 0" << std::endl;
    std::cout << "(4) Go to end" << std::endl;
    std::cout << "(5) exit" << std::endl;
    int x;
    std::cin >> x;
    return x;
}
