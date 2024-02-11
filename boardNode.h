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
    int lastDoublePawnMoveIndex;
    CastleStatus castleStatus;
    unordered_map<int, U64> pins;
    unordered_map<int, U64> opponentPins;
    multimap<double, Move, greater<double>> moves;
    vector<BoardNode*> children;
    double value;
    public:
    // Board* getBoard(); // TEMP
    bool moveListEmpty(); // TEMP
    BoardNode(Board* board, int lastDoublePawnMoveIndex, CastleStatus castleStatus, unordered_map<int, U64> opponentPins);
    U64 getColourPieces(Colour colour);
    double staticEval();
    void searchLegalMusks(Colour colour, unordered_map<int, U64>& pins, bool& check, U64& checkPath, bool& doubleCheck, U64& kingLegalMoves, int& kingSquare);
    bool isSquareSafe(int square, Colour colour);
    U64 generateUnsafeMusk(Colour teamColour);
    void generateMoves(Colour colour);
    void addPredictedBestMove(Colour colour);
    friend ostream& operator<<(ostream& out, BoardNode& boardNode);
    ostream& printBoardOnly(ostream& out);
    ostream& printChildren(ostream& out);
    ostream& printChildrenMoveNotation(ostream& out);
    vector<BoardNode*> getChildren();
    ~BoardNode();
    void deleteChildren();
    void clearMoves();
    void deleteBoard();
    bool containsMove(int fromSquare, int toSquare);
    friend void branchToChild(BoardNode*& boardNode, size_t index);
    void setValue(double val);
    double getValue() const;
};
#endif
