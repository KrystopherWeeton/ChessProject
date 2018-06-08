#ifndef King_H
#define King_H

class King : public Piece {
private:
public:
    King(bool isWhite, Location location) : Piece(isWhite, -1, location) { }
    King() : Piece() { }
    
    char getIdentifier() const {
        return 'K';
    }
    
    friend std::ostream& operator <<(std::ostream& output, const King& k) {
        output << k.getIdentifier();
        return output;
    }
};

#endif
