#include "boardNode.h"
#include "constants.h"
#include "player.h"
#include "invalidUserMove.h"
class User: public Player {
    public:
    User(Colour colour);
    void pickMove(unique_ptr<BoardNode>& pos);
};

