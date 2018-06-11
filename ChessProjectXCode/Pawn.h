#ifndef Pawn_H
#define Pawn_H

#include <iostream>
#include "Piece.h"

class Pawn : public Piece {
private:
public:
    
    Pawn(bool isWhite, Location location) : Piece(isWhite, 1, location) { }
    Pawn() : Piece() { }
    
    virtual char getIdentifier() const {
        return 'P';
    }
    
    friend std::ostream& operator<<(std::ostream& output, const Pawn& p) {
        output << p.getIdentifier();
        return output;
    }
    
};

#endif
