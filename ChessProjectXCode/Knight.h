#ifndef Knight_H
#define Knight_H

class Knight : public Piece {
private:
public:
    Knight(bool isWhite, Location location) : Piece(isWhite, 3, location) { }
    Knight() : Piece() { }
    
    char getIdentifier() const {
        return 'N';
    }
    
    friend std::ostream& operator<<(std::ostream& output, const Knight& k) {
        output << k.getIdentifier();
        return output;
    }
};

#endif
