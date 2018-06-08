#ifndef Bishop_H
#define Bishop_H

class Bishop : public Piece {
private:
public:
    Bishop(bool isWhite, Location location) : Piece(isWhite, 3, location) { }
    Bishop() : Piece() { }
    
    char getIdentifier() const {
        return 'B';
    }
    
    friend std::ostream& operator<<(std::ostream& output, const Bishop& b) {
        output << b.getIdentifier();
        return output;
    }
};

#endif
