#ifndef ChessBoard_H
#define ChessBoard_H

#include "PieceSet.h"
#include "DecodeReturn.h"
#include "Move.h"

enum Legality {
    Legal,
    KingInCheck,        //Secondary
    PutsKingInCheck,    //Takes Precedence
    CantCastle
};

class ChessBoard {
private:
    
    PieceSet white = PieceSet(true);
    PieceSet black = PieceSet(false);
    Piece* board[8][8];
    
    //An integer which represents the last lane where a pawn moved forward two
    //Reset to -1 when the last move did not move a pawn forward 2
    int pawnStartingLane;
    
    static const std::string resetColor;
    static const std::string labelColor;
    static const std::string whiteColor;
    static const std::string blackColor;
    static const std::string darkSquareColor;
    static const std::string lightSquareColor;
    
    bool whiteKingHasMoved = false;
    bool blackKingHasMoved = false;
    
    //Helper functions for the print function below
    void printWhite(std::ostream& output);
    void printBlack(std::ostream& output);
    
    
    
    //Sets the specified location to the piece, returning true if a piece was overwritten
    bool set(Location l, Piece* p);
    
    //Clears the given square, without doing any memory clean up
    void clear(Location l) {
        board[l.x][l.y] = nullptr;
    }
    
    
    //Checks the legality of a move
    Legality isLegal(bool whitesTurn, Move);
    
    //Performs a move, ASSUMING IT IS LEGAL
    void performMove(const Move& m);
    
    friend class AnalysisManager;
    
public:
    
    ChessBoard();
    
    bool kingInCheck(bool whitesKing);
    
    //Does the move, then tests the result based off the func passed, and finally restores the board
    //Note: This is the only function which modifies the state of the board, without actually updating the pieces
    bool testMove(Move m, std::function<bool()> func);
    
    // Gathers the locations the piece could move from to take this
    std::vector<Location> gatherFromLocations(int x, int y, char iden, bool whiteTurn);
    
    int getPawnStartingLane() const {
        return pawnStartingLane;
    }
    
    void setPawnStartingLane(int x) {
        pawnStartingLane = x;
    }
    
    std::vector<Location> getLegalMoves(Piece* p);
    
    void print(bool whitesPerspective, std::ostream& output);
    
    Piece* at(int x, int y) {
        if (!isValidLocation(x, y))
            return nullptr;
        return board[x][y];
    }
    Piece* at(Location l) {
        return at(l.x, l.y);
    }
    
    
    bool canCastle(bool whiteTurn, bool king);
    
    // Note: Assumes that canCastle has been called and the pieces can successfully castle
    void castle(bool whiteTurn, bool king);
    
    //Checks square with varying behaviour for return values of true from func
    std::vector<Location> checkSurroundingSquares(Location location, std::function<bool(Location)> func);
    std::vector<Location> checkDiagonals(Location location, std::function<bool(Location)> func);
    std::vector<Location> checkLines(Location location, std::function<bool(Location)> func);
    std::vector<Location> checkKnightMoves(Location location, std::function<bool(Location)> func);
    std::vector<Location> checkPawnMoves(Location location, std::function<bool(Location)> func, bool isWhite);
    std::vector<Location> checkQueenMoves(Location location, std::function<bool(Location)> func);
    
    //Wrapper functions to make it easier to check squares for specific pieces
    std::vector<Location> checkSurroundingSquares(Location location, std::vector<char> chars, bool isWhite);
    std::vector<Location> checkDiagonals(Location location, std::vector<char> chars, bool isWhite);
    std::vector<Location> checkKnightMoves(Location location, std::vector<char> chars, bool isWhite);
    std::vector<Location> checkLines(Location location, std::vector<char> chars, bool isWhite);
    std::vector<Location> checkPawnMoves(Location location, std::vector<char> chars, bool iswhite);
    std::vector<Location> checkQueenMoves(Location location, std::vector<char> chars, bool isWhite);

    
    //Wrapper functions to make it easier to check existential statements for pieces
    bool boolCheckSurroundingSquares(Location location, std::vector<char> chars, bool isWhite);
    bool boolCheckDiagonals(Location location, std::vector<char> chars, bool isWhite);
    bool boolCheckKnightMoves(Location location, std::vector<char> chars, bool isWhite);
    bool boolCheckLines(Location location, std::vector<char> chars, bool isWhite);
    bool boolCheckPawnMoves(Location location, std::vector<char> chars, bool isWhite);
    bool boolCheckQueenMoves(Location location, std::vector<char> chars, bool isWhite);
    
    //Gathers all legal moves for a piece set
    std::vector<Move> gatherAllLegalMoves(bool isWhite);
    
    //Returns a boolean representing whether or not the piece can be taken
    bool canBeTaken(Location location);
    bool canBeTakenBy(Location location, bool isWhite);
    
    //Checks whether the king can take the location, without putting itself into check
    bool kingCanTake(Location location, bool whitesKing);
    
    //Resets the board to starting position
    void reset();
    
    //Checks if the given square is empty
    bool isEmpty(int x, int y) const {
        if (!isValidLocation(x, y))
            return false;
        return board[x][y] == nullptr;
    }
    bool isEmpty(Location l) const {
        return isEmpty(l.x, l.y);
    }
    
    //Checks for the identifier at the given location
    bool idenAt(int x, int y, char& c) const;
    bool idenAt(Location l, char& c) const;
    char idenAt(Location l) const;
    
    //Checks if the piece at the given location is white
    bool isWhite(Location l) const;
    
    //Finds the location of whites king on the board
    Location findKing(bool whitesKing) const;
    
    //Checks if the location given is within the board
    bool isValidLocation(int x, int y) const {
        return (x >= 0 && x <=7 && y >= 0 && y <=7);
    }
    bool isValidLocation(Location l) const {
        return isValidLocation(l.x, l.y);
    }
    
    //Checks the legality of the move, and then performs it
    Legality doMove(bool whitesTurn, const Storable& m, int& points);
};

#endif
