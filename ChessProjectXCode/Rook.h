#ifndef Rook_H
#define Rook_H

#include <iostream>
#include "Piece.h"

class Rook : public Piece {
private:
public:
    Rook(bool isWhite, Location location) : Piece(isWhite, 5, location) { }
    Rook() : Piece() { }
    
    virtual char getIdentifier() const {
        return 'R';
    }
    
    friend std::ostream& operator <<(std::ostream& output, const Rook& r) {
        output << r.getIdentifier();
        return output;
    }
};

#endif
