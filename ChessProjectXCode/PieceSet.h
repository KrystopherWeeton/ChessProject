#ifndef PieceSet_H
#define PieceSet_H

#include "Piece.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include <stdlib.h>

#include "Move.h"
#include <vector>

class PieceSet {
private:
    Queen q;    //Held in [14]
    King k;     //Held in [15]
    Rook r1;
    Rook r2;
    Bishop b1;
    Bishop b2;
    Knight n1;
    Knight n2;
    Pawn pawns [ 8 ];
    std::vector<Piece*> extraPieces;
    
    //********** Helper Functions **********
    
public:
    PieceSet(bool isWhite) {
        int startingY = (isWhite) ? 0 : 7;
        int modifier = (isWhite) ? 1 : -1;
        
        //Creates pawns
        for (int i = 0; i < 8; i++) {
            pawns[i] = Pawn(isWhite, Location(i, startingY + modifier));
        }
        
        //Creates Rooks
        r1 = Rook(isWhite, Location(0, startingY));
        r2 = Rook(isWhite, Location(7, startingY));
        
        //Creates Knights
        n1 = Knight(isWhite, Location(1, startingY));
        n2 = Knight(isWhite, Location(6, startingY));
        
        //Creates Bishops
        b1 = Bishop(isWhite, Location(2, startingY));
        b2 = Bishop(isWhite, Location(5, startingY));
        
        //Creates King and Queen
        q = Queen(isWhite, Location(3, startingY));
        k = King(isWhite, Location(4, startingY));
    }
    
    ~PieceSet() {
        for (auto it = extraPieces.begin(); it != extraPieces.end();) {
            Piece* p = *it;
            it++;
            delete p;
        }
    }
    
    //Used when a pawn turns into a queen
    Queen* addQueen(Location l, bool isWhite) {
        Queen* q = new Queen(isWhite, l);
        extraPieces.push_back(q);
        return q;
    }
    
    //Used when a pawn turns into a knight
    Knight* addKnight(Location l, bool isWhite) {
        Knight * n = new Knight(isWhite, l);
        extraPieces.push_back(n);
        return n;
    }
    
    bool isWhite() const {
        return k.isWhite();
    }
    
    Location kingLocation() const {
        return k.getLocation();
    }
    
    void reset() {
        *this = PieceSet(isWhite());
        extraPieces.clear();
    }
    
    void forEveryActivePiece(const std::function<void(Piece*)> func) {
        for (int i = 0; i < 8; i++)
            if (pawns[i].isActive())
                func(&pawns[i]);
        
        if(k.isActive())
            func(&k);
        if(q.isActive())
            func(&q);
        if(b1.isActive())
            func(&b1);
        if(b2.isActive())
            func(&b2);
        if(n1.isActive())
            func(&n1);
        if(n2.isActive())
            func(&n2);
        if(r1.isActive())
            func(&r1);
        if(r2.isActive())
            func(&r2);
        
        //Runs the function on all dynamic pieces
        for (auto it = extraPieces.begin(); it != extraPieces.end(); it++)
            func(*it);
    }
    
    Pawn* findDeactivatedPawn() {
        for (int i = 0; i < 8; i ++)
            if (!pawns[i].isActive())
                return &pawns[i];
        return nullptr;
    }
    
};

#endif
