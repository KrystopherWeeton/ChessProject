#ifndef Queen_H
#define Queen_H

class Queen : public Piece {
private:
public:
    Queen(bool isWhite, Location location) : Piece(isWhite, 9, location) { }
    Queen() : Piece() { }
    
    char getIdentifier() const {
        return 'Q';
    }
    
    friend std::ostream& operator<<(std::ostream& output, const Queen& q) {
        output << q.getIdentifier();
        return output;
    }
};

#endif
