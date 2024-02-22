#ifndef LOOKUP_TABLE
#define LOOKUP_TABLE
#include "maskUtils.h"
#include <unordered_map>
#include <vector>
#include "constants.h"
#include <memory>
#include <unordered_set>
#include <chrono>
#include <fstream>
using namespace std;
using namespace MaskUtils;
using namespace chrono;
class LookupTable {
    static U64 whitePawnMusks[64];
    static U64 blackPawnMusks[64];
    static U64 whitePawnControlMusks[64];
    static U64 blackPawnControlMusks[64];
    static U64 bishopMusks[64];
    static U64 rookMusks[64];
    static U64 queenMusks[64];
    static U64 knightMusks[64];
    static U64 kingMusks[64];
    static U64 rayMusks[8][64];

    // magic number arrays
    static int whitePawnBlockerShifts[64];
    static int blackPawnBlockerShifts[64];
    static int bishopBlockerShifts[64];
    static int rookBlockerShifts[64];

    static U64 whitePawnMagicArray[64];
    static U64 blackPawnMagicArray[64];
    static U64 bishopMagicArray[64];
    static U64 rookMagicArray[64];

    static U64 rookMagicMoves[64][4096];
    static U64 bishopMagicMoves[64][512];
    static U64 whitePawnMagicMoves[64][16];
    static U64 blackPawnMagicMoves[64][16];

    const static int UPINDEX = 0;
    const static int DOWNINDEX = 1;
    const static int LEFTINDEX = 2;
    const static int RIGHTINDEX = 3;
    const static int UPRIGHTINDEX = 4;
    const static int DOWNLEFTINDEX = 5;
    const static int UPLEFTINDEX = 6;
    const static int DOWNRIGHTINDEX = 7;
    
    public:
    static U64 lookupPawnControlMusk(int key, Colour colour);
    static U64 lookupMusk(int key, Piece piece);
    static U64 lookupMove(int square, Piece piece, U64 allPieces);
    static U64 lookupRayMusk(int square, int direction);
    static void setRayMusks();
    static void setMusks();
    static U64* generateBlockerMusks(U64 movementMusk);
    static void mapBlockerKeys();
};
#endif
