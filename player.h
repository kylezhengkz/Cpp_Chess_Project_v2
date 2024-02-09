#ifndef PLAYER_H
#define PLAYER_H
#include "constants.h"
#include "boardNode.h"
class Player {
    protected:
    Colour colour;
    public:
    Player(Colour colour);
    Colour getColour();
    virtual void pickMove(BoardNode*& pos) = 0; // DEBUGGING PURPOSES
};
#endif
