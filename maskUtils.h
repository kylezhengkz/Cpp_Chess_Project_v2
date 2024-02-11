#ifndef MASKUTIL_H
#define MASKUTIL_H
#include <cstdint>
#include <iostream>
using namespace std;
namespace MaskUtils {
    typedef uint64_t U64;
    const U64 A1 = 0x1ULL;
    const U64 A2 = 0x2ULL;
    const U64 A3 = 0x4ULL;
    const U64 A4 = 0x8ULL;
    const U64 A5 = 0x10ULL;
    const U64 A6 = 0x20ULL;
    const U64 A7 = 0x40ULL;
    const U64 A8 = 0x80ULL;

    const U64 B1 = 0x100ULL;
    const U64 B2 = 0x200ULL;
    const U64 B3 = 0x400ULL;
    const U64 B4 = 0x800ULL;
    const U64 B5 = 0x1000ULL;
    const U64 B6 = 0x2000ULL;
    const U64 B7 = 0x4000ULL;
    const U64 B8 = 0x8000ULL;

    const U64 C1 = 0x10000ULL;
    const U64 C2 = 0x20000ULL;
    const U64 C3 = 0x40000ULL;
    const U64 C4 = 0x80000ULL;
    const U64 C5 = 0x100000ULL;
    const U64 C6 = 0x200000ULL;
    const U64 C7 = 0x400000ULL;
    const U64 C8 = 0x800000ULL;

    const U64 D1 = 0x1000000ULL;
    const U64 D2 = 0x2000000ULL;
    const U64 D3 = 0x4000000ULL;
    const U64 D4 = 0x8000000ULL;
    const U64 D5 = 0x10000000ULL;
    const U64 D6 = 0x20000000ULL;
    const U64 D7 = 0x40000000ULL;
    const U64 D8 = 0x80000000ULL;

    const U64 E1 = 0x100000000ULL;
    const U64 E2 = 0x200000000ULL;
    const U64 E3 = 0x400000000ULL;
    const U64 E4 = 0x800000000ULL;
    const U64 E5 = 0x1000000000ULL;
    const U64 E6 = 0x2000000000ULL;
    const U64 E7 = 0x4000000000ULL;
    const U64 E8 = 0x8000000000ULL;

    const U64 F1 = 0x10000000000ULL;
    const U64 F2 = 0x20000000000ULL;
    const U64 F3 = 0x40000000000ULL;
    const U64 F4 = 0x80000000000ULL;
    const U64 F5 = 0x100000000000ULL;
    const U64 F6 = 0x200000000000ULL;
    const U64 F7 = 0x400000000000ULL;
    const U64 F8 = 0x800000000000ULL;

    const U64 G1 = 0x1000000000000ULL;
    const U64 G2 = 0x2000000000000ULL;
    const U64 G3 = 0x4000000000000ULL;
    const U64 G4 = 0x8000000000000ULL;
    const U64 G5 = 0x10000000000000ULL;
    const U64 G6 = 0x20000000000000ULL;
    const U64 G7 = 0x40000000000000ULL;
    const U64 G8 = 0x80000000000000ULL;

    const U64 H1 = 0x100000000000000ULL;
    const U64 H2 = 0x200000000000000ULL;
    const U64 H3 = 0x400000000000000ULL;
    const U64 H4 = 0x800000000000000ULL;
    const U64 H5 = 0x1000000000000000ULL;
    const U64 H6 = 0x2000000000000000ULL;
    const U64 H7 = 0x4000000000000000ULL;
    const U64 H8 = 0x8000000000000000ULL;

    const U64 A_FILE = 0x0101010101010101ULL;
    const U64 B_FILE = 0x0202020202020202ULL;
    const U64 C_FILE = 0x0404040404040404ULL;
    const U64 D_FILE = 0x0808080808080808ULL;
    const U64 E_FILE = 0x1010101010101010ULL;
    const U64 F_FILE = 0x2020202020202020ULL;
    const U64 G_FILE = 0x4040404040404040ULL;
    const U64 H_FILE = 0x8080808080808080ULL;

    const U64 RANK_1 = 0x00000000000000FFULL;
    const U64 RANK_2 = 0x000000000000FF00ULL;
    const U64 RANK_3 = 0x0000000000FF0000ULL;
    const U64 RANK_4 = 0x00000000FF000000ULL;
    const U64 RANK_5 = 0x000000FF00000000ULL;
    const U64 RANK_6 = 0x0000FF0000000000ULL;
    const U64 RANK_7 = 0x00FF000000000000ULL;
    const U64 RANK_8 = 0xFF00000000000000ULL;

    // bit shift
    const int VERTICAL = 8;
    const int HORIZONTAL = 1;
    const int NEGATIVE_DIAGONAL = 7;
    const int POSITIVE_DIAGONAL = 9;
    const int NO_DIRECTION = 0;

    int getBit(const U64& b, int i);
    void setBit(U64& b, int i);
    void clearBit(U64& b, int i);
    int getLSB(const U64& b);
    int getMSB(const U64& b);
    int popLSB(U64& b);
    U64 clearBitsGreaterThanIndex(U64 b, int index);
    U64 clearBitsLessThanIndex(U64 b, int index);
    ostream& printBitboard(const U64& board, ostream& out);
    bool isDiagonal(int direction);
};
#endif
