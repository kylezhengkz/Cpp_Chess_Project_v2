#ifndef BOARDNODE_H
#define BOARDNODE_H
#include "board.h"
#include <vector>
#include <map>
#include "lookupTable.h"
#include <random>
#include "move.h"
#include "castleStatus.h"
class BoardNode {
    string id;
    Board* board;
    double value;
    int lastDoublePawnMoveIndex;
    CastleStatus castleStatus;
    unordered_map<int, U64> opponentPins;
    multimap<double, Move> moves;
    vector<BoardNode*> children;
    public:
    BoardNode(Board* board, double value, int lastDoublePawnMoveIndex, CastleStatus castleStatus, unordered_map<int, U64> opponentPins);
    U64 getColourPieces(Colour colour);
    double staticEval();
    void searchLegalMusks(Colour colour, unordered_map<int, U64>& pins, bool& check, U64& checkPath, bool& doubleCheck);
    U64 generateUnsafeMusk(Colour teamColour);
    void generateMoves(Colour colour);
    void branchPredictedBestMove(Colour colour);
    friend ostream& operator<<(ostream& out, BoardNode& boardNode);
    ostream& printBoardOnly(ostream& out);
    ostream& printChildren(ostream& out);
    vector<BoardNode*> getChildren();
    void setNewValue(double newValue);
    double getValue() const;
    ~BoardNode();
    void deleteChildren();
    void deleteBoard();
    friend void branchToChild(BoardNode*& boardNode, size_t index);
};
#endif
