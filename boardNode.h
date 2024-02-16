#ifndef BOARDNODE_H
#define BOARDNODE_H
#include "board.h"
#include <vector>
#include <map>
#include "lookupTable.h"
#include <random>
#include "move.h"
#include "castleStatus.h"
#include <iterator>
class BoardNode {
    Board* board;
    int lastDoublePawnMoveIndex;
    CastleStatus castleStatus;
    unordered_map<int, U64> pins;
    unordered_map<int, U64> opponentPins;
    multimap<double, Move, greater<double>> moves;
    vector<BoardNode*> children;
    double value;
    U64 checkPathMusk;
    // TEMP (debugging purposes only)
    BoardNode* parent;
    public:
    // Board* getBoard(); // TEMP
    bool moveListEmpty(); // TEMP
    BoardNode(Board* board, int lastDoublePawnMoveIndex, CastleStatus castleStatus, unordered_map<int, U64> opponentPins, BoardNode* parent);
    U64 getColourPieces(Colour colour);
    double staticEval();
    void searchLegalMusks(Colour colour, bool& check, bool& doubleCheck, U64& kingLegalMoves, int& kingSquare, U64& pinBlockMusk);
    void searchNewPinsOnly(Colour colour, int oldKingSquare, int newKingSquare, unordered_map<int, U64> newPins);
    bool isSquareSafe(int square, Colour colour);
    U64 generateUnsafeMusk(Colour teamColour, bool print);
    void generateMoves(Colour colour);
    void addPredictedBestMove(Colour colour);
    friend ostream& operator<<(ostream& out, BoardNode& boardNode);
    ostream& printBoardOnly(ostream& out);
    ostream& printChildrenValues(ostream& out);
    ostream& printChildrenMoveNotation(ostream& out);
    ostream& printChildrenTree(ostream& out);
    ostream& printChildrenValue(ostream& out);
    vector<BoardNode*>& getChildren();
    ~BoardNode();
    void deleteChildren();
    void clearMoves();
    void deleteBoard();
    bool containsMove(int fromSquare, int toSquare);
    friend void branchToChild(BoardNode*& boardNode, size_t index);
    void setValue(double val);
    double getValue() const;
    multimap<double, Move, greater<double>>& getMoves();
};
#endif
