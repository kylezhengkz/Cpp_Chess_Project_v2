#ifndef STARTPOSITION_H
#define STARTPOSITION_H
#include "boardNode.h"
class StartPosition {
    public:
    static BoardNode* defaultPosition(Board* board);
    static BoardNode* manualSetup(Board* board);
};
#endif
