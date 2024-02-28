#ifndef STARTPOSITION_H
#define STARTPOSITION_H
#include "boardNode.h"
class StartPosition {
   public:
    static unique_ptr<BoardNode> defaultPosition(unique_ptr<Board> &board);
    static unique_ptr<BoardNode> castleTestSetup(unique_ptr<Board> &board);
    static unique_ptr<BoardNode> manualSetup(unique_ptr<Board> &board);
    static unique_ptr<BoardNode> setFromOutput(unique_ptr<Board> &board);
};
#endif
