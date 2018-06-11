#include "ChessBoard.h"
#include "Location.h"
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include "Storable.h"

const std::string ChessBoard::resetColor =  "\033[0m";                 //default color
const std::string ChessBoard::labelColor = "\033[1m\033[31m";       //Bold Red
const std::string ChessBoard::whiteColor = "\033[1m\033[37m";       //Bold White
const std::string ChessBoard::blackColor = "\033[1m\033[30m";       //Black
const std::string ChessBoard::darkSquareColor = "\033[42m" ;        //Green
const std::string ChessBoard::lightSquareColor = resetColor ;       //Basic Color

/*
 Prints out the board from the white point of view
 Output - The ostream for the board to be output
 */
void ChessBoard::printWhite(std::ostream& output) {
    for (int y = 7; y >= 0; y--) {
        output << labelColor << (y + 1) << ' ';
        for (int x = 0; x < 8; x++) {
            Piece* p = board[x][y];
            output << (((x + y) % 2 == 0) ? darkSquareColor : lightSquareColor);
            if (p == nullptr || !p->isActive()) {
                output << "  ";
            } else {
                output << (p->isWhite() ? whiteColor : blackColor) << p->getIdentifier() << ' ';
            }
        }
        output << resetColor << labelColor << '*' << std::endl;
    }
    output << "* A B C D E F G H *" << resetColor << std::endl;
}

/*
 Castles one of the side's pieces manually
 whiteTurn - Is it white's turn? This specifies which pieces we are going to castle.
 king - whether we are castling king side
 Pre: Castling is legal at this point
 */
void ChessBoard::castle(bool whiteTurn, bool king) {
    
    
    int y = (whiteTurn) ? 0 : 7;
    
    Location kingLocation = Location( 4, y );
    Location rookLocation = Location((king ? 7 : 0), y);
    
    
    //Moves the pieces
    // Note: During this piont in the code, we are assuming that (a) the pieces havent been moved, (b) the
    // pieces are in the correct location, (c) there are not pieces blocking the, and (c) no placed between the
    // two pieces are currently being threatened
    Location newKingLocation((king) ? 6 : 2, y);
    Location newRookLocation((king) ? 5 : 3, y);
    
    Move kingMove(kingLocation, newKingLocation);
    Move rookMove(rookLocation, newRookLocation);
    
    performMove(kingMove);
    performMove(rookMove);
}

/*
 Determines if the correct pieces can castle
 king - whether we are castling king side
 whiteTurn - Is it white's turn? This specifies which pieces we are going to castle.
 */
bool ChessBoard::canCastle(bool whiteTurn, bool king) {
    //Checks that the king is not currently threatened
    if (this->kingInCheck(whiteTurn))
        return false;
    
    // Determines whether we are castling king's / queen's side
    int y = (whiteTurn) ? 0 : 7;
    
    // Checks whether the king has moved previously
    if ((whiteTurn ? whiteKingHasMoved : blackKingHasMoved) == true)
        return false;
    
    // Checks the king and rooks are where they should be
    
    Location kingLocation = Location( 4, y );
    Location rookLocation = Location((king ? 7 : 0), y);
    
    Piece* rook = at(rookLocation);
    
    if (findKing(whiteTurn) != kingLocation)
        return false;
    else if (rook == nullptr || rook->getIdentifier() != 'R')
        return false;
    
    //Puts the squares between the king and rook into the array to parse
    std::vector<Location> locations;
    if (king) {
        locations.push_back(Location(5, y));
        locations.push_back(Location(6, y));
    } else {
        locations.push_back(Location(1, y));
        locations.push_back(Location(2, y));
        locations.push_back(Location(3, y));
    }
    
    //Checks those squares, and short circuits if necessary
    
    for (auto it = locations.begin(); it != locations.end(); it++) {
        if (at(*it) != nullptr || canBeTakenBy(*it, whiteTurn))
            return false;
    }
    
    return true;
}

/*
 Sets the piece at location l to p
 l - the location of the piece to be set.
 p - the piece to be placed at the locatoin
 */
bool ChessBoard::set(Location l, Piece* p) {
    Piece* replaced = at(l);
    board[l.x][l.y] = p;
    if (p != nullptr)
        p->setLocation(l);
    if (replaced != nullptr) {
        replaced->deactivate();
        replaced->setLocation(Location(-1, -1));
    }
    //Note: The pointers are merely references to the PieceSet pointers, so no memory leak occurs
    return replaced != nullptr;
}

/*
 Prints the board from black's point of view
 output - the ostream to print the board to
 */
void ChessBoard::printBlack(std::ostream& output) {
    for (int y = 0; y < 8; y++) {
        output << labelColor << (y + 1) << ' ';
        for (int x = 7; x >= 0; x--) {
            Piece* p = board[x][y];
            output << (((x + y) % 2 == 0) ? darkSquareColor : lightSquareColor);
            if (p == nullptr || !p->isActive()) {
                output << "  ";
            } else {
                output << (p->isWhite() ? whiteColor : blackColor) << p->getIdentifier() << ' ';
            }
        }
        output << resetColor << labelColor << '*' << std::endl;
    }
    output << "* H G F E D C B A *" << resetColor << std::endl;
}

/*
 Constructs the Chess Board
 */
ChessBoard::ChessBoard() {
    //Resets all the pieces
    reset();
    
}

/*
 Prints the Chess Board from the given perspective
 whitesPerspective - Whether to display the board from whitesPerspective
 output - the ostream for the board
 */
void ChessBoard::print(bool whitesPerspective, std::ostream& output) {
    output << labelColor << "* * * * * * * * * *" << std::endl;
    (whitesPerspective) ? printWhite(output) : printBlack(output);
}

/*
 Gathers all legal moves for the piece given
 p - the piece to gather the moves for
 */
std::vector<Location> ChessBoard::getLegalMoves(Piece* p) {
    std::vector<Location> locations;
    if (p == nullptr)
        return locations;
    
    bool isWhite = p->isWhite();
    Location loc = p->getLocation();
    
    switch (p->getIdentifier()) {
        case 'P': {
            // We must handle three cases, (1) Forward Movement, (2) En Passant, (3) Diagonal Taking
            
            int mod = (isWhite) ? 1 : -1;
            
            //Checks foward movement of 1
            if (isEmpty(loc.x, loc.y + mod))
                locations.push_back(Location(loc.x, loc.y + mod));
            
            //Checks forward movement of 2
            if (locations.size() == 1 && isEmpty(loc.x, loc.y + mod * 2))
                locations.push_back(Location(loc.x, loc.y + mod * 2));
                
            //Checks diagonal taking
            if (!isEmpty(loc.x + 1, loc.y + mod) ||     //Diagonal Taking
                (getPawnStartingLane() == loc.x + 1 && ( (loc.y == 4 && isWhite) || (loc.y == 3 && !isWhite) )))    //En Passant
                locations.push_back(Location(loc.x + 1, loc.y + mod));
            
            
            Location fin(loc.x - 1, loc.y + mod);
            if (isValidLocation(fin) && ( !isEmpty(fin) ||
                (getPawnStartingLane() == loc.x - 1 && ( (loc.y == 4 && isWhite) || (loc.y == 3 && !isWhite) ))))
                locations.push_back(Location(loc.x - 1, loc.y + mod));
            
            break;
        }
        case 'R': {
            locations = checkLines(loc, [this, isWhite] (Location l) -> bool {
                return ( isEmpty(l.x, l.y) || (at(l.x, l.y)->isWhite() != isWhite) );
            });
            break;
        }
        case 'B': {
            locations = checkDiagonals(loc, [this, isWhite] (Location l) -> bool {
                return ( isEmpty(l.x, l.y) || (at(l.x, l.y)->isWhite() != isWhite) );
            });
            break;
        }
        case 'N': {
            locations = checkKnightMoves(loc, [this, isWhite] (Location l) -> bool {
                return ( isEmpty(l.x, l.y) || (at(l.x, l.y)->isWhite() != isWhite) );
            });
            break;
        }
        case 'Q': {
            locations = checkQueenMoves(loc, [this, isWhite] (Location l) -> bool {
                return ( isEmpty(l.x, l.y) || (at(l.x, l.y)->isWhite() != isWhite) );
            });
            break;
        }
        case 'K': {
            locations = checkSurroundingSquares(loc, [this, isWhite] (Location l) -> bool {
                return kingCanTake(l, isWhite);
            });
            break;
        }
    }
    
    return locations;
}

/*
 Examines surrounding squares using the function passed in
 location - the central location
 func - the function to be used
 */
std::vector<Location> ChessBoard::checkSurroundingSquares(Location location, std::function<bool (Location)> func) {
    std::vector<Location> takenFrom;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++ ) {
            if ((x != 0 || y != 0) && isValidLocation(location.x + x, location.y + y)) {
                Location l(location.x + x, location.y + y);
                if (func(l))
                    takenFrom.push_back(l);
            }
        }
    }
    return takenFrom;
}

/*
 Examines the diagonals from the functino passed in until the func returns false
 location - the central location
 func - the function which processes the squares
 */
std::vector<Location> ChessBoard::checkDiagonals(Location location, std::function<bool(Location)> func) {
    std::vector<Location> takenFrom;
    int x = location.x;
    int y = location.y;
    
    //Upwards right
    int max = std::min(7 - x, 7 - y);
    for (int i = 1; i <= max; i++) {
        Location l(x + i, y + i);
        if (func(l))
            takenFrom.push_back(l);
        if (at(l) != nullptr)    //We only hit this if the function returns false
            break;
    }
    
    //Upwards left
    max = std::min(x, 7 - y);
    for (int i = 1; i <= max; i++) {
        Location l(x - i, y + i);
        if (func(l))
            takenFrom.push_back(l);
        if (at(l) != nullptr)
            break;
    }
    
    //Downards left
    max = std::min(x, y);
    for (int i = 1; i <= max; i++) {
        Location l(x - i, y - i);
        if (func(l))
            takenFrom.push_back(l);
        if (at(l) != nullptr)
            break;
    }
    
    //Downards Right
    max = std::min(7 - x, y);
    for (int i = 1; i <= max; i++) {
        Location l(x + i, y - i);
        if (func(l))
            takenFrom.push_back(l);
        if (at(l) != nullptr)
            break;
    }
    
    return takenFrom;
}

std::vector<Location> ChessBoard::checkLines(Location location, std::function<bool(Location)> func) {
    std::vector<Location> takenFrom;
    int x = location.x;
    int y = location.y;
    
    //Leftward
    for (int i = 1; x - i >= 0; i++) {
        Location l(x - i, y);
        if (func(l))
            takenFrom.push_back(l);
        if (at(l) != nullptr)
            break;
    }
    
    //Rightward
    for (int i = 1; x + i <= 7; i++) {
        Location l(x + i, y);
        if (func(l))
            takenFrom.push_back(l);
        else if (board[x + i][y] != nullptr)
            break;
    }
    
    //Downward
    for (int i = 1; y - i >= 0; i++) {
        Location l(x, y - i);
        if (func(l))
            takenFrom.push_back(l);
        if (at(l) != nullptr)
            break;
    }
    
    //Upward
    for (int i = 1; y + i <= 7; i++) {
        Location l(x, y + i);
        if (func(l))
            takenFrom.push_back(l);
        if (at(l) != nullptr)
            break;
    }
    
    return takenFrom;
}

std::vector<Location> ChessBoard::checkKnightMoves(Location location, std::function<bool(Location)> func) {
    std::vector<Location> takenFrom;
    int x = location.x;
    int y = location.y;
    
    //Check vertical squares
    for (int x2 = -2; x2 <= 2; x2 += 4) {
        for (int y2 = -1; y2 <= 1; y2 += 2 ) {
            Location l(x + x2, y + y2);
            if (isValidLocation(l) && func(l))
                takenFrom.push_back(l);
        }
    }
    
    //Check horizontal squares
    for (int x3 = -1; x3 <=1; x3 += 2) {
        for (int y3 = -2; y3 <= 2; y3 += 2) {
            Location l(x + x3, y + y3);
            if (isValidLocation(l) && func(l))
                takenFrom.push_back(l);
        }
    }
    
    return takenFrom;
}

std::vector<Location> ChessBoard::checkPawnMoves(Location location, std::function<bool(Location)> func, bool isWhite) {
    int mod = (isWhite) ? -1 : 1;
    
    Location op1(location.x + 1, location.y + mod);
    Location op2(location.x - 1, location.y + mod);
    
    std::vector<Location> takenFrom;
    
    if (isValidLocation(op1) && at(op1) != nullptr && func(op1))
        takenFrom.push_back(op1);
    if (isValidLocation(op2) && at(op2) != nullptr && func(op2))
        takenFrom.push_back(op2);
    
    return takenFrom;
}

std::vector<Location> ChessBoard::checkQueenMoves(Location location, std::function<bool(Location)> func) {
    std::vector<Location> locations = checkLines(location, func);
    std::vector<Location> secondary = checkDiagonals(location, func);
    locations.insert(locations.end(), secondary.begin(), secondary.end());
    return locations;
}


std::vector<Location> ChessBoard::checkSurroundingSquares(Location location, std::vector<char> chars, bool isWhite) {
    return checkSurroundingSquares(location, [chars, this, isWhite] (Location l) -> bool {
        Piece* p = board[l.x][l.y];
        if (p == nullptr || p->isWhite() == isWhite)
            return false;
        return std::find(chars.begin(), chars.end(), p->getIdentifier()) != chars.end();
    });
}

std::vector<Location> ChessBoard::checkDiagonals(Location location, std::vector<char> chars, bool isWhite) {
    return checkDiagonals(location, [chars, this, isWhite] (Location l) -> bool {
        Piece* p = board[l.x][l.y];
        if (p == nullptr || p->isWhite() == isWhite)
            return false;
        return std::find(chars.begin(), chars.end(), p->getIdentifier()) != chars.end();
    });
}

std::vector<Location> ChessBoard::checkKnightMoves(Location location, std::vector<char> chars, bool isWhite) {
    return checkKnightMoves(location, [chars, this, isWhite] (Location l) -> bool {
        Piece* p = board[l.x][l.y];
        if (p == nullptr || p->isWhite() == isWhite)
            return false;
        return std::find(chars.begin(), chars.end(), p->getIdentifier()) != chars.end();
    });
}

std::vector<Location> ChessBoard::checkLines(Location location, std::vector<char> chars, bool isWhite) {
    return checkLines(location, [chars, this, isWhite] (Location l) -> bool {
        Piece* p = board[l.x][l.y];
        if (p == nullptr || p->isWhite() == isWhite)
            return false;
        return std::find(chars.begin(), chars.end(), p->getIdentifier()) != chars.end();
    });
}

std::vector<Location> ChessBoard::checkPawnMoves(Location location, std::vector<char> chars, bool isWhite) {
    return checkPawnMoves(location, [chars, this, isWhite] (Location l) -> bool {
        Piece* p = board[l.x][l.y];
        if (p == nullptr || p->isWhite() == isWhite)
            return false;
        return std::find(chars.begin(), chars.end(), p->getIdentifier()) != chars.end();
    }, isWhite);
}

std::vector<Location> ChessBoard::checkQueenMoves(Location location, std::vector<char> chars, bool isWhite) {
    return checkQueenMoves(location, [chars, this, isWhite] (Location l) -> bool {
        Piece* p = board[l.x][l.y];
        if (p == nullptr || p->isWhite() == isWhite)
            return false;
        return std::find(chars.begin(), chars.end(), p->getIdentifier()) != chars.end();
    });
}

bool ChessBoard::canBeTaken(Location location) {
    
    Piece* p = at(location);
    
    //Grab a bool for the color of piece we are looking for
    bool color = p->isWhite();
    
    //Checks surrounding squares for something that can potentially take its
    std::vector<Location> kings = checkSurroundingSquares(location, {'K'}, !color);  //Check for opp. color king
    if (kings.size() != 0) {
        if (kings.size() > 1) {
            std::cout << "SOMEONE FUCKED UP AND ALLOWED TWO KINGS ONTO THE BOARD" << std::endl;
            return false;
        }
        
        Location kingLocation = kings[0];
        if (kingCanTake(location, !color))    //Checks if the king of opp color can take (without giving check)
            return true;
    }
    
    return ( boolCheckDiagonals(location, {'Q', 'B'}, color) || // Checks for a piece on the diagonals which can capture
            boolCheckLines(location, {'Q', 'R'}, color) ||      // Checks for a piece on the lines which can capture
            boolCheckKnightMoves(location, {'N'}, color) ||     // Checks for a piece on the knightMoves which can capture
            boolCheckPawnMoves(location, {'P'}, color)         // Checks for a piece on the pawnMoves which can capture
    );
}

bool ChessBoard::canBeTakenBy(Location location, bool isWhite) {
    //Checks surrounding squares for something that can potentially take its
    std::vector<Location> kings = checkSurroundingSquares(location, {'K'}, !isWhite);  //Check for opp. color king
    if (kings.size() != 0) {
        if (kings.size() > 1) {
            std::cout << "SOMEONE FUCKED UP AND ALLOWED TWO KINGS ONTO THE BOARD" << std::endl;
            return false;
        }
        
        Location kingLocation = kings[0];
        if (kingCanTake(location, !isWhite))    //Checks if the king of opp color can take (without giving check)
            return true;
    }
    
    return ( boolCheckDiagonals(location, {'Q', 'B'}, isWhite) || // Checks for a piece on the diagonals which can capture
            boolCheckLines(location, {'Q', 'R'}, isWhite) ||      // Checks for a piece on the lines which can capture
            boolCheckKnightMoves(location, {'N'}, isWhite) ||     // Checks for a piece on the knightMoves which can capture
            boolCheckPawnMoves(location, {'P'}, isWhite)         // Checks for a piece on the pawnMoves which can capture
            );
}

bool ChessBoard::kingCanTake(Location location, bool whitesKing) {
    Location kingLocation = findKing(whitesKing);
    
    //Check bullshit cases
    if (at(kingLocation) == nullptr || at(kingLocation)->getIdentifier() != 'K')    //Called with no king at kingLocation
        return false;
    if (std::abs(kingLocation.x - location.x) > 1 || std::abs(kingLocation.y - location.y) > 1)
        return false;       //King can't actually move to this location
    
    //Checks surrounding squares of the move for a king
    if (boolCheckSurroundingSquares(location, {'K'}, !whitesKing) == false)
        return false;
    
    // Checks the location you are moving to for a piece of the same color
    Piece* p = at(location);
    if (p != nullptr && p->isWhite() == whitesKing)
        return false;
    
    bool b = testMove(Move(kingLocation, location), [this, location, whitesKing] () -> bool {
        return !kingInCheck(whitesKing);
    });
    
    //Return the value we grabbed
    return b;
}

void ChessBoard::reset() {
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            board[x][y] = nullptr;
    
    white.reset();
    black.reset();
    
    //Sets the locations on the board
    auto func = [this] (Piece* p) {
        Location l = p->getLocation();
        board[l.x][l.y] = p;
    };
    white.forEveryActivePiece(func);
    black.forEveryActivePiece(func);
    
    whiteKingHasMoved = false;
    blackKingHasMoved = false;
}

bool ChessBoard::idenAt(int x, int y, char& c) const {
    if (board[x][y] == nullptr)
        return false;
    c = board[x][y]->getIdentifier();
    return true;
}

bool ChessBoard::idenAt(Location l, char& c) const {
    return idenAt(l.x, l.y, c);
}

char ChessBoard::idenAt(Location l) const {
    char c = '0';
    idenAt(l, c);
    return c;
}

bool ChessBoard::isWhite(Location l) const {
    return (board[l.x][l.y] != nullptr && board[l.x][l.y]->isWhite());
}

Location ChessBoard::findKing(bool whitesKing) const{
    return (whitesKing ? white : black).kingLocation();
}

//Checks the legality of a move
Legality ChessBoard::isLegal(bool whitesTurn, Move m) {

    // Checks for the edge case of the move being a castle
    if (m == KING_CASTLE || m == QUEEN_CASTLE) {
        if (canCastle(whitesTurn, m == KING_CASTLE))
            return Legality::Legal;
        else
            return Legality::CantCastle;
    }
    
    //Checks if movement puts king into check / keeps the king in check
    bool putsKingInCheck = testMove(m, [this, whitesTurn] () -> bool {
        // Gathers the king's location again as it may be the piece that is moving
        return this->kingInCheck(whitesTurn);
    });
    
    //Uses the booleans above to differentiate between case (a) king is in check (b) king becomes checked through movement, or (c) the move is legal
    if (putsKingInCheck)
        return Legality::PutsKingInCheck;
    return Legality::Legal;
}

std::vector<Location> ChessBoard::gatherFromLocations(int x, int y, char iden, bool whiteTurn) {
    
    std::vector<Location> locations;
    
    //This is where we check the possible locations the move could have come from
    //Note: We do not check the legality of the move, only attempt to identify the piece it came from
    //Secondary Note: We allow moves from -1, and then remove them later. This is a hit to performance, but
    //Makes my work a lot easier
    
    std::function<bool(Location l)> func = [whiteTurn, this, iden] (Location l) -> bool {
        return (at(l) != nullptr &&
                at(l)->isWhite() == whiteTurn &&
                at(l)->getIdentifier() == iden);
    };
    
    
    switch (iden) {
        case 'P': {
            // We must handle three cases, (1) Forward Movement, (2) En Passant, (3) Diagonal Taking
            
            int yMod = (whiteTurn) ? -1 : 1;
            
            if (!isEmpty(x, y)) {    //(3) Diagonal Taking or (2) En Passant
                
                
                //Diagonal Taking
                locations.push_back(Location(x + 1, y + yMod));
                locations.push_back(Location(x - 1, y + yMod));
                
                
            } else {        //(1) Forward Movement
                locations.push_back(Location(x, y + yMod));
                
                //En Passant
                if (getPawnStartingLane() == x &&
                    ( (y == 5 && whiteTurn) || (y == 3 && !whiteTurn) ) ) {    // En Passant is a possibility
                    locations.push_back(Location(x - 1, y + yMod));
                    locations.push_back(Location(x + 1, y + yMod));
                }
                
                //Must handle double movement here
                if ((y == 3 && whiteTurn) || (y == 4 && !whiteTurn)) {  //We can jump forward 2
                    //NOTE: WE MUST TAKE CARE TO CHECK NOTHING IS IN OUR WAY
                    locations.push_back(Location(x, y + yMod * 2));
                }
            }
            break;
        }
        case 'R': {
            locations = checkLines(Location(x, y), func);
            break;
        }
        case 'N': {
            locations = checkKnightMoves(Location(x, y), func);
            break;
        }
        case 'B': {
            locations = checkDiagonals(Location(x, y), func);
            break;
        }
        case 'Q': {
            locations = checkQueenMoves(Location(x, y), func);
            break;
        }
        case 'K': {
            locations = checkSurroundingSquares(Location(x, y), func);
            break;
        }
    }
    
    
    // Cleanses the locations vector to include only positions with the identifier passed in
    locations.erase(std::remove_if(locations.begin(), locations.end(), [this] (Location l) -> bool {
        return this->at(l) == nullptr;
    }), locations.end());
    
    return locations;
}


bool ChessBoard::kingInCheck(bool whitesKing) {
    return canBeTaken(findKing(whitesKing));
}


bool ChessBoard::testMove(Move m, std::function<bool ()> func) {
    //Save State
    Piece* to = at(m.to);
    Piece* from = at(m.from);
    
    //Moves the pieces
    board[m.to.x][m.to.y] = from;
    board[m.from.x][m.from.y] = nullptr;
    if (to != nullptr)
        to->deactivate();
    if (from != nullptr)
        from->setLocation(m.to);
    
    //Checks the result
    bool b = func();
    
    //Restores the board
    board[m.to.x][m.to.y] = to;
    board[m.from.x][m.from.y] = from;
    if (to != nullptr)
        to->activate();
    if (from != nullptr)
        from->setLocation(m.from);
    
    return b;
}

Legality ChessBoard::doMove(bool whitesTurn, const Storable &m, int& points) {
    
    // Checks the legality of the move and returns the appropriate result
    Legality isMoveLegal = isLegal(whitesTurn, m);
    if (isMoveLegal != Legality::Legal)
        return isMoveLegal;
    
    // Determines if we are castling, and then castles if it is
    if (m == KING_CASTLE || m == QUEEN_CASTLE) {
        castle(whitesTurn, m == KING_CASTLE);
    }
    
    //Adds the points taken to the specified point total
    if(!isEmpty(m.to)) {
        points += (at(m.to))->getValue();
    }
    
    //Checks for pawn double forward, to update EnPassant tracker
    if (m.to.x == m.from.x && std::abs(m.to.y - m.from.y) == 2 && idenAt(m.from) == 'P') {  // We did a double move
        pawnStartingLane = m.to.x;
    } else {                                                                                // No double move
        pawnStartingLane = -1;
    }
    
    //Checks if we are doing an En-Passant
    if (std::abs(m.to.x - m.from.x) == 1 &&                 // Check we are moving one to the right / left
        m.to.y == m.from.y + ((whitesTurn) ? 1 : -1) &&     // Check we are moving forward 1
        isEmpty(m.to) && idenAt(m.from) == 'P') {           // Check the square we are moving to is empty, and we are moving a pawn
        //The only time the pawn moves diagonally, without taking a piece, is en passant so we know we are in en passant here
        //Additionally, we know that the pawn being taken is on the same x as the to, and down / up one
        
        // Manually handles all updates to value
        Location l(m.to.x, m.to.y + ( (whitesTurn) ? -1 : 1) );
        clear(l);
        points += 1;
    }
    
    // Gathers the piece at the location we are moving from
    Piece* piece = at(m.from);
    
    //Checks for turning a pawn into a queen
    if ( (m.to.y == 7 && whitesTurn) || (m.to.y == 0 && !whitesTurn) ) {    //Some piece is moving to the end
        if (piece != nullptr && piece->getIdentifier() == 'P')  {           //The pawn is moving to the end
            
            // Performs the move
            performMove(m);
            
            // deactivates the pawn
            piece->deactivate();
            
            // Sets the piece to a queen. Does not allow for knights because it complicates analysis manager too much
            this->set(m.to, (whitesTurn ? white : black).addQueen(m.to, whitesTurn));
            
            return isMoveLegal;
        }
    }
    
    //Moves the pieces
    performMove(m);
    
    //We should be able to theoretically return Success; however, this approach keeps us safe
    return isMoveLegal;
}

void ChessBoard::performMove(const Move& m) {
    Piece* p = at(m.from);
    if (p != nullptr && p->getIdentifier() == 'K')
        (p->isWhite()) ? whiteKingHasMoved : blackKingHasMoved = true;
    //Moves the pieces
    set(m.to, at(m.from));
    clear(m.from);  //The pointer is moved, so does not need to be cleaned
}

std::vector<Move> ChessBoard::gatherAllLegalMoves(bool isWhite) {
    std::vector<Move> moves;
    PieceSet p = (isWhite ? white : black);
    p.forEveryActivePiece([&moves, this, isWhite] (Piece* p) {
        
        std::vector<Location> secondary = this->getLegalMoves(p);
        
        
        for (auto it = secondary.begin(); it != secondary.end(); it++)
            if (this->testMove(Move(p->getLocation(), *it), [this, isWhite] () -> bool {
                return !this->kingInCheck(isWhite);
            }))
                moves.push_back(Move(p->getLocation(), *it));
        
    });
    return moves;
}


bool ChessBoard::boolCheckSurroundingSquares(Location location, std::vector<char> chars, bool isWhite) {
    return (checkSurroundingSquares(location, chars, isWhite)).size() != 0;
}

bool ChessBoard::boolCheckDiagonals(Location location, std::vector<char> chars, bool isWhite) {
    return (checkDiagonals(location, chars, isWhite)).size() != 0;
}

bool ChessBoard::boolCheckKnightMoves(Location location, std::vector<char> chars, bool isWhite) {
    return (checkKnightMoves(location, chars, isWhite)).size() != 0;
}

bool ChessBoard::boolCheckLines(Location location, std::vector<char> chars, bool isWhite) {
    return (checkLines(location, chars, isWhite)).size() != 0;
}

bool ChessBoard::boolCheckPawnMoves(Location location, std::vector<char> chars, bool isWhite) {
    return (checkPawnMoves(location, chars, isWhite)).size() != 0;
}

bool ChessBoard::boolCheckQueenMoves(Location location, std::vector<char> chars, bool isWhite) {
    return (checkQueenMoves(location, chars, isWhite)).size() != 0;
}




