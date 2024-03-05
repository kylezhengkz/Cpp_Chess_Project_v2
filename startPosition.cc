#include "startPosition.h"
unique_ptr<BoardNode> StartPosition::kingSetup(unique_ptr<Board> &board) {
    setBit(board->whiteKing, 62);
    setBit(board->blackKing, 60);
    CastleStatus castleStatus(false, false, false, false);
    unique_ptr<BoardNode> boardNode(new BoardNode(move(board), -1, castleStatus, nullptr));
    return boardNode;
}

unique_ptr<BoardNode> StartPosition::defaultPosition(unique_ptr<Board> &board) {
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
    CastleStatus castleStatus(true, true, true, true);
    unique_ptr<BoardNode> boardNode(new BoardNode(move(board), -1, castleStatus, nullptr));
    return boardNode;
}

unique_ptr<BoardNode> StartPosition::castleTestSetup(unique_ptr<Board> &board) {
    setBit(board->whiteRooks, 0);
    setBit(board->whiteRooks, 7);
    setBit(board->blackRooks, 56);
    setBit(board->blackRooks, 63);
    setBit(board->whiteKing, 4);
    setBit(board->blackKing, 60);
    unordered_map<int, U64> emptyMap;
    CastleStatus castleStatus(true, true, true, true);
    unique_ptr<BoardNode> boardNode(new BoardNode(move(board), -1, castleStatus, nullptr));
    return boardNode;
}

unique_ptr<BoardNode> StartPosition::manualSetup(unique_ptr<Board> &board) {
    setBit(board->whiteKing, 4);
    setBit(board->whiteBishops, 2);
    setBit(board->whiteBishops, 26);
    setBit(board->whitePawns, 51);
    setBit(board->whitePawns, 8);
    setBit(board->whitePawns, 9);
    setBit(board->whitePawns, 10);
    setBit(board->whitePawns, 14);
    setBit(board->whitePawns, 15);
    setBit(board->whiteKnights, 12);
    setBit(board->whiteKnights, 1);
    setBit(board->whiteRooks, 0);
    setBit(board->whiteRooks, 7);
    setBit(board->whiteQueens, 3);

    setBit(board->blackKing, 61);
    setBit(board->blackRooks, 63);
    setBit(board->blackRooks, 56);
    setBit(board->blackKnights, 57);
    setBit(board->blackBishops, 58);
    setBit(board->blackBishops, 52);
    setBit(board->blackQueens, 59);
    setBit(board->blackKnights, 13);
    setBit(board->blackPawns, 48);
    setBit(board->blackPawns, 49);
    setBit(board->blackPawns, 42);
    setBit(board->blackPawns, 53);
    setBit(board->blackPawns, 54);
    setBit(board->blackPawns, 55);
    CastleStatus castleStatus(true, false, false, false);
    unique_ptr<BoardNode> boardNode(new BoardNode(move(board), -1, castleStatus, nullptr));
    boardNode->printBoardOnly(cout);
    return boardNode;
}
