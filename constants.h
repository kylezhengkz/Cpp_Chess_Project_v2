#ifndef CONSTANTS_H
#define CONSTANTS_H
enum class Colour { WHITE, BLACK };
inline Colour operator!(Colour colour) {
    if (colour == Colour::WHITE) {
        return Colour::BLACK;
    } else {
        return Colour::WHITE;
    }
}
enum class Piece { WHITEPAWN, BLACKPAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
#endif
