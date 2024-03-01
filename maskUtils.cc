#include "maskUtils.h"
MaskUtils::U64 MaskUtils::clearBitsEqualGreaterThanIndex(U64 b, int index) {
    U64 musk = (1ULL << index) - 1;
    return b & musk;
}

MaskUtils::U64 MaskUtils::clearBitsEqualLessThanIndex(U64 b, int index) {
    U64 musk = ~(((1ULL << index) - 1) | (0x1ULL << index));
    return b & musk;
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

MaskUtils::U64 MaskUtils::generateRandomU64Range(int lowestNumOfBits, int highestNumOfBits) {  // note that it generates a U64 with 6-12 nonzero bits
    random_device rd;
    mt19937_64 eng(rd());

    uniform_int_distribution<> bitCountDistr(lowestNumOfBits, highestNumOfBits);
    int bitsToSet = bitCountDistr(eng);

    uniform_int_distribution<> bitPositionDistr(0, 63);

    U64 result = 0;
    while (bitsToSet > 0) {
        result |= U64(1) << bitPositionDistr(eng);
        bitsToSet--;
    }

    return result;
}

MaskUtils::U64 MaskUtils::generateRandomU64Exact(int numOfNonZeroBits) {
    random_device rd;
    mt19937_64 eng(rd());

    uniform_int_distribution<> bitPositionDistr(0, 63);

    U64 result = 0;
    while (numOfNonZeroBits > 0) {
        U64 newBit = U64(1) << bitPositionDistr(eng);
        if ((result & newBit) == 0) {
            result |= newBit;
            numOfNonZeroBits--;
        }
    }

    return result;
}
