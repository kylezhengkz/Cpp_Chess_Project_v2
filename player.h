#ifndef PLAYER_H
#define PLAYER_H
#include "boardNode.h"
#include "constants.h"
class Player {
   protected:
    Colour colour;

   public:
    Player(Colour colour);
    Colour getColour();
    virtual void pickMove(unique_ptr<BoardNode> &pos) = 0;
};
#endif
