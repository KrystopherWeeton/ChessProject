#ifndef Location_H
#define Location_H

#include <string>
#include <iostream>

//Represents an offset from A1
class Location {
private:
public:
    const int x;    // The X position 0 - 7
    const int y;    // The Y position 0 - 7
    
    Location(int x = 0, int y = 0) : x(x), y(y) {}
    
    Location(const Location& rhs) : x(rhs.x), y(rhs.y) { }
    
    Location operator=(const Location& rhs) {
        (int&)x = rhs.x;
        (int&)y = rhs.y;
        return *this;
    }
    
    bool operator==(Location& rhs) {
        return x == rhs.x && y == rhs.y;
    }
    
    bool operator!=(const Location& rhs) {
        return ! (*this == rhs);
    }
    
    bool friend operator==(Location& lhs, const Location& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    
    bool friend operator==(const Location& lhs, const Location& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    
    friend std::ostream& operator<<(std::ostream& output, Location rhs) {
        output << (char)(97 + rhs.x) << (rhs.y + 1);
        return output;
    }
    
    operator std::string() const {
        return {(char)(97 + x), (char)(y + '1')};
    }
};

#endif
