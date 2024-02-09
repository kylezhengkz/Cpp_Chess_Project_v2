#ifndef MOVE_H
#define MOVE_H
#include "maskUtils.h"
struct Move {
    int16_t move;
    static const int noFlag = 0b0000;
    static const int castle = 0b0001;
    static const int enPassant = 0b0010;
    static const int pawnDoubleMove = 0b0011;
    static const int queenPromotion = 0b0100;
    static const int rookPromotion = 0b0101;
    static const int knightPromotion = 0b0110;
    static const int bishopPromotion = 0b0111;
    static const int pawnMove = 0b1000;
    static const int knightMove = 0b1001;
    static const int bishopMove = 0b1010;
    static const int rookMove = 0b1011;
    static const int queenMove = 0b1100;
    static const int kingMove = 0b1101;
    int8_t capture;
    static const int noCapture = 0b0000;
    static const int pawnCapture = 0b0001;
    static const int knightCapture = 0b0010;
    static const int bishopCapture = 0b0011;
    static const int rookCapture = 0b0100;
    static const int queenCapture = 0b0101;
    Move(int fromSquare, int toSquare, int flag, int8_t capture);
    int getFromSquare();
    int getToSquare();
    int getFlag();
    int getCapture();
};
#endif
