#ifndef LOOKUP_TABLE
#define LOOKUP_TABLE
#include "maskUtils.h"
#include <unordered_map>
#include <vector>
#include "constants.h"
#include <memory>
using namespace std;
using namespace MaskUtils;
class LookupTable {
    static U64* whitePawnMusks;
    static U64* blackPawnMusks;
    static U64* whitePawnControlMusks;
    static U64* blackPawnControlMusks;
    static U64* bishopMusks;
    static U64* rookMusks;
    static U64* queenMusks;
    static U64* knightMusks;
    static U64* kingMusks;
    const static int UPINDEX = 0;
    const static int DOWNINDEX = 1;
    const static int LEFTINDEX = 2;
    const static int RIGHTINDEX = 3;
    const static int UPRIGHTINDEX = 4;
    const static int DOWNLEFTINDEX = 5;
    const static int UPLEFTINDEX = 6;
    const static int DOWNRIGHTINDEX = 7;
    static U64 rayMusks[8][64];
    struct PairHash {
        template <class T1, class T2>
        size_t operator() (const pair<T1, T2>& p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    struct PairEqual {
        template <class T1, class T2>
        bool operator() (const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) const {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };
    static unordered_map<pair<int, U64>, U64, PairHash, PairEqual> whitePawnMoves;
    static unordered_map<pair<int, U64>, U64, PairHash, PairEqual> blackPawnMoves;
    static unordered_map<pair<int, U64>, U64, PairHash, PairEqual> bishopMoves;
    static unordered_map<pair<int, U64>, U64, PairHash, PairEqual> rookMoves;
    static unordered_map<pair<int, U64>, U64, PairHash, PairEqual> queenMoves;
    public:
    static U64 lookupPawnControlMusk(int key, Colour colour);
    static U64 lookupMusk(int key, Piece piece);
    static U64 lookupMove(int square, Piece piece, U64 allPieces);
    static U64 lookupRayMusk(int square, int direction);
    static void setRayMusks();
    static void setMusks();
    static U64* generateBlockerMusks(U64 movementMusk);
    static void mapBlockerKeys();
    static void cleanup();
};
#endif
