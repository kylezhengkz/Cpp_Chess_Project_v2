#include "startPosition.h"
BoardNode* StartPosition::defaultPosition(Board* board) {
    for (int i = 8; i < 16; i++) {
        setBit(board->whitePawns, i);
    }
    for (int i = 48; i < 56; i++) {
        setBit(board->blackPawns, i);
    }

    setBit(board->whiteRooks, 0);
    setBit(board->whiteRooks, 7);
    setBit(board->blackRooks, 56);
    setBit(board->blackRooks, 63);
    setBit(board->whiteKnights, 1);
    setBit(board->whiteKnights, 6);
    setBit(board->blackKnights, 57);
    setBit(board->blackKnights, 62);
    setBit(board->whiteBishops, 2);
    setBit(board->whiteBishops, 5);
    setBit(board->blackBishops, 58);
    setBit(board->blackBishops, 61);
    setBit(board->whiteQueens, 3);
    setBit(board->blackQueens, 59);
    setBit(board->whiteKing, 4);
    setBit(board->blackKing, 60);
    board->whitePieces = board->whitePawns | board->whiteKnights | board->whiteBishops | board->whiteRooks | board->whiteQueens | board->whiteKing;
    board->blackPieces = board->blackPawns | board->blackKnights | board->blackBishops | board->blackRooks | board->blackQueens | board->blackKing;
    unordered_map<int, U64> emptyMap;
    CastleStatus castleStatus(true, true, true, true);
    BoardNode* boardNode = new BoardNode(board, -1, castleStatus, emptyMap);
    return boardNode;
}

BoardNode* StartPosition::manualSetup(Board* board) {
    unordered_map<int, U64> emptyMap;
    CastleStatus castleStatus(true, true, true, true);
    BoardNode* boardNode = new BoardNode(board, -1, castleStatus, emptyMap);
    return boardNode;
}
