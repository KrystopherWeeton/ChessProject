#ifndef SpecMoves
#define SpecMoves

enum Spec {
    kingCastle, queenCastle, queenPromotion, knightPromotion, leave
};

class SpecMove: public Storable {
private:
    Spec move;
public:
    SpecMove(Spec spec) : move(spec) { }
    
    Spec getSpec() const { return move; }
};

#endif
