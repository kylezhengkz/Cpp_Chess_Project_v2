#ifndef CASTLESTATUS_H
#define CASTLESTATUS_H
#include "maskUtils.h"
struct CastleStatus {
    uint8_t castleStatus;
    CastleStatus(bool whiteRC, bool whiteLC, bool blackRC, bool blackLC);
    bool canWhiteKingCastleRight();
    bool canWhiteKingCastleLeft();
    bool canBlackKingCastleRight();
    bool canBlackKingCastleLeft();
    void disenableWhiteKingCastleRight();
    void disenableWhiteKingCastleLeft();
    void disenableBlackKingCastleRight();
    void disenableBlackKingCastleLeft();
    void printStatuses();
};
#endif
