#include "castleStatus.h"
CastleStatus::CastleStatus(bool whiteRC, bool whiteLC, bool blackRC, bool blackLC) {
    castleStatus = whiteRC | (whiteLC << 1) | (blackRC << 2) | (blackLC << 3);
}

bool CastleStatus::canWhiteKingCastleRight() {
    return (castleStatus & 0b0001) != 0;
}

bool CastleStatus::canWhiteKingCastleLeft() {
    return (castleStatus & 0b0010) != 0;
}

bool CastleStatus::canBlackKingCastleRight() {
    return (castleStatus & 0b0100) != 0;
}

bool CastleStatus::canBlackKingCastleLeft() {
    return (castleStatus & 0b1000) != 0;
}

void CastleStatus::disenableWhiteKingCastleRight() { castleStatus &= ~0b0001; }

void CastleStatus::disenableWhiteKingCastleLeft() { castleStatus &= ~0b0010; }

void CastleStatus::disenableBlackKingCastleRight() { castleStatus &= ~0b0100; }

void CastleStatus::disenableBlackKingCastleLeft() { castleStatus &= ~0b1000; }

void CastleStatus::printStatuses() {
    cout << "Can white king castle right? "
         << ((canWhiteKingCastleRight() == true) ? "Yes" : "No") << endl;
    cout << "Can white king castle right? "
         << ((canWhiteKingCastleLeft() == true) ? "Yes" : "No") << endl;
    cout << "Can black king castle right? "
         << ((canBlackKingCastleRight() == true) ? "Yes" : "No") << endl;
    cout << "Can black king castle right? "
         << ((canBlackKingCastleLeft() == true) ? "Yes" : "No") << endl;
}
