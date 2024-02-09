#include "board.h"
Board::Board() {}

U64 Board::getPieces(Piece piece, Colour colour) {
    if (colour == Colour::WHITE) {
        switch (piece) {
            case (Piece::WHITEPAWN):
                return whitePawns;
                break;
            case (Piece::KNIGHT):
                return whiteKnights;
                break;
            case (Piece::BISHOP):
                return whiteBishops;
                break;
            case (Piece::ROOK):
                return whiteRooks;
                break;
            case (Piece::QUEEN):
                return whiteQueens;
                break;
            case (Piece::KING):
                return whiteKing;
                break;
            default:
                throw logic_error("Attempted to get an invalid piece");
                break;
        }
    } else {
        switch (piece) {
            case (Piece::BLACKPAWN):
                return blackPawns;
                break;
            case (Piece::KNIGHT):
                return blackKnights;
                break;
            case (Piece::BISHOP):
                return blackBishops;
                break;
            case (Piece::ROOK):
                return blackRooks;
                break;
            case (Piece::QUEEN):
                return blackQueens;
                break;
            case (Piece::KING):
                return blackKing;
                break;
            default:
                throw logic_error("Attempted to get an invaild piece");
                break;
        }
    }
}

double Board::findPiece(int squareIndex, Colour teamColour, int8_t& captureFlag) {
    if (teamColour == Colour::WHITE) {
        if (getBit(whitePawns, squareIndex)) {
            captureFlag = Move::pawnCapture;
            return 1;
        } else if (getBit(whiteKnights, squareIndex)) {
            captureFlag = Move::bishopCapture;
            return 3;
        } else if (getBit(whiteBishops, squareIndex)) {
            captureFlag = Move::knightCapture;
            return 3.05;
        } else if (getBit(whiteRooks, squareIndex)) {
            captureFlag = Move::rookCapture;
            return 5;
        } else if (getBit(whiteQueens, squareIndex)) {
            captureFlag = Move::queenCapture;
            return 9;
        } else {
            throw logic_error("Expected a capture but did not find one");
        }
    } else {
        if (getBit(blackPawns, squareIndex)) {
            captureFlag = Move::pawnCapture;
            return 1;
        } else if (getBit(blackKnights, squareIndex)) {
            captureFlag = Move::knightCapture;
            return 3;
        } else if (getBit(blackBishops, squareIndex)) {
            captureFlag = Move::bishopCapture;
            return 3.05;
        } else if (getBit(blackRooks, squareIndex)) {
            captureFlag = Move::rookCapture;
            return 5;
        } else if (getBit(blackQueens, squareIndex)) {
            captureFlag = Move::queenCapture;
            return 9;
        } else {
            throw logic_error("Expected a capture but did not find one");
        }
    }
}

double Board::getPieceValue(Piece piece) {
    switch (piece) {
        case (Piece::WHITEPAWN):
            return 1;
            break;
        case (Piece::BLACKPAWN):
            return 1;
            break;
        case (Piece::BISHOP):
            return 3;
            break;
        case (Piece::KNIGHT):
            return 3.05;
            break;
        case (Piece::ROOK):
            return 5;
            break;
        case (Piece::QUEEN):
            return 9;
            break;
        default:
            throw logic_error("Cannot get piece value");
            break;
    }
}

void printLetterRow(ostream& out) {
    out << "  ";
    for (char c = 'A'; c <= 'H'; c++) {
        out << c;
    }
    out << endl;
}

void printDashRow(ostream& out) {
    out << "  ";
    for (int i = 0; i < 8; i++) {
        out << '-';
    }
    out << endl;
}

ostream& operator<<(ostream& out, Board& board) {
    printLetterRow(out);
    printDashRow(out);
    for (int i = 56; i >= 0; i-=8) {
        out << 8 - ((56 - i)/8) << '|';
        for (int j = 0; j < 8; j++) {
            int index = i + j;
            if (getBit(board.whitePawns, index)) {
                out << 'P';
            } else if (getBit(board.blackPawns, index)) {
                out << 'p';
            } else if (getBit(board.whiteRooks, index)) {
                out << 'R';
            } else if (getBit(board.blackRooks, index)) {
                out << 'r';
            } else if (getBit(board.whiteKnights, index)) {
                out << 'N';
            } else if (getBit(board.blackKnights, index)) {
                out << 'n';
            } else if (getBit(board.whiteBishops, index)) {
                out << 'B';
            } else if (getBit(board.blackBishops, index)) {
                out << 'b';
            } else if (getBit(board.whiteQueens, index)) {
                out << 'Q';
            } else if (getBit(board.blackQueens, index)) {
                out << 'q';
            } else if (getBit(board.whiteKing, index)) {
                out << 'K';
            } else if (getBit(board.blackKing, index)) {
                out << 'k';
            } else {
                out << '-';
            }
        }
        out << '|' << 8 - ((56 - i)/8);
        out << endl;
    }
    printDashRow(out);
    printLetterRow(out);
    return out;
}
