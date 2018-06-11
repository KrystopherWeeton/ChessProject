#ifndef Move_H
#define Move_H

#include "Location.h"
#include "Storable.h"
#include <string>

class Move: public Storable {
public:
    const Location from;    // The location we are moving from
    const Location to;      // The location we are moving to
    
    Move(Location from = Location(0,0), Location to = Location(0,0)) : from(from), to(to) { }
    
    Move(const Move& move) : from(move.from), to(move.to) { }
    
    Move operator=(const Move& rhs) {
        (Location&)from = rhs.from;
        (Location&)to = rhs.to;
        return *this;
    }
    
    bool operator==(const Move& rhs) {
        return from == rhs.from && to == rhs.to;
    }
    
    friend bool operator==(const Move& lhs, const Move& rhs) {
        return lhs.from == rhs.from && lhs.to == rhs.to;
    }
    
    friend std::ostream& operator<<(std::ostream& output, const Move& move) {
        output << "From<" << move.from << "> To<" << move.to << ">";
        return output;
    }
};

#endif
