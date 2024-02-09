#include "maskUtils.h"
int MaskUtils::getBit(const U64& b, int i) {
    return (b & (1ULL << i)) != 0;
}

void MaskUtils::setBit(U64& b, int i) {
    b |= (1ULL << i);
}

void MaskUtils::clearBit(U64& b, int i) {
    b &= ~(1ULL << i);
}

int MaskUtils::getLSB(const U64& b) {
    if (b == 0) {
        return -1;
    }
    return __builtin_ctzll(b);
}

int MaskUtils::getMSB(const U64& b) {
    if (b == 0) {
        return -1;
    }
    return 63 - (__builtin_clzll(b));
}

int MaskUtils::popLSB(U64& b) {
    int i = getLSB(b);
    b &= b - 1;
    return i;
}

MaskUtils::U64 MaskUtils::clearBitsGreaterThanIndex(U64 b, int index) {
    U64 musk = (1ULL << (index + 1)) - 1;
    return b & musk;
}

MaskUtils::U64 MaskUtils::clearBitsLessThanIndex(U64 b, int index) {
    U64 mask = ~((1ULL << index) - 1);
    return b & mask;
}

ostream& MaskUtils::printBitboard(const U64& board, ostream& out) {
    for (int i = 56; i >= 0; i -= 8) {
        for (int j = 0; j < 8; j++) {
            out << getBit(board, (i + j));
        }
        out << endl;
    }
    out << endl;
    return out;
}

bool MaskUtils::isDiagonal(int direction) {
    if (abs(direction) == NEGATIVE_DIAGONAL || abs(direction) == POSITIVE_DIAGONAL) {
        return true;
    } else {
        return false;
    }
}
