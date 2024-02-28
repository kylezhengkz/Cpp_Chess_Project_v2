#ifndef BOARD_H
#define BOARD_H
#include "constants.h"
#include "maskUtils.h"
#include "move.h"
using namespace std;
using namespace MaskUtils;
struct Board {
    U64 whitePawns = 0x0ULL;
    U64 whiteKnights = 0x0ULL;
    U64 whiteBishops = 0x0ULL;
    U64 whiteRooks = 0x0ULL;
    U64 whiteQueens = 0x0ULL;
    U64 whiteKing = 0x0ULL;
    U64 blackPawns = 0x0ULL;
    U64 blackKnights = 0x0ULL;
    U64 blackBishops = 0x0ULL;
    U64 blackRooks = 0x0ULL;
    U64 blackQueens = 0x0ULL;
    U64 blackKing = 0x0ULL;
    Board();
    U64 getPieces(Piece piece, Colour colour);  // abstraction purposes
    double findPiece(int squareIndex, Colour teamColour, int8_t& captureFlag);
    double getPieceValue(Piece piece);
    U64 getWhitePiecesMusk();
    U64 getBlackPiecesMusk();
    friend ostream& operator<<(ostream& out, Board& board);
};
#endif
