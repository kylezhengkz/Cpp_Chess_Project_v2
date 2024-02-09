#include "move.h"
Move::Move(int fromSquare, int toSquare, int flag, int8_t capture): capture{capture} {
    move = fromSquare | (toSquare << 6) | (flag << 12);
}

int Move::getFromSquare() {
    return move & 0b111111;
}

int Move::getToSquare() {
    return (move >> 6) & 0b111111;
}

int Move::getFlag() {
    return (move >> 12) & 0b1111;
}

int Move::getCapture() {
    return capture;
}
