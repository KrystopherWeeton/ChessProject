#ifndef Piece_H
#define Piece_H

#include "Location.h"

enum Color {
    white,
    black
};

class Piece {
private:
    const int value = 0;
    Color color;
    bool active = true;
    Location location;
public:
    
    Piece(): value(0), color(Color::white), active(false), location(Location(0,0)) { }
    
    Piece(bool isWhite, int value, Location location) : value(value), location(location) {
        color = (isWhite ? Color::white : Color::black);
    }
    
    Piece(const Piece& other): value(other.value), color(other.color), active(other.active), location(other.location) { }
    
    void deactivate() {
        active = false;
    }
    
    void activate() {
        active = true;
    }
    
    bool isActive() const {
        return active;
    }
    
    virtual ~Piece() { };
    
    bool isWhite() const {
        return color == Color::white;
    }
    
    virtual char getIdentifier() const = 0;
    
    //Outputs a piece's identifier to the output
    friend std::ostream& operator<<(std::ostream& output, Piece p) {
        output << "One of the pieces does not overload output operator";
        return output;
    }
    
    int getValue() const {
        return value;
    }
    
    Location getLocation() const {
        return location;
    }
    
    void setLocation(Location l) {
        location = l;
    }
    
    Piece& operator=(const Piece& rhs) {
        (int&)value = rhs.value;
        color = rhs.color;
        active = rhs.active;
        location = rhs.location;
        return *this;
    }
    
};

#endif
