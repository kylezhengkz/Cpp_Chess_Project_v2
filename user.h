#include "boardNode.h"
#include "constants.h"
#include "player.h"
class User : public Player {
   public:
    User(Colour colour);
    void pickMove(unique_ptr<BoardNode>& pos);
};
