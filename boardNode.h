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
    unique_ptr<Board> board;
    int lastDoublePawnMoveIndex;
    CastleStatus castleStatus;
    unordered_map<int, U64> pins;
    unordered_map<int, U64> opponentPins;
    multimap<double, unique_ptr<Move>, greater<double>> moves;
    vector<unique_ptr<BoardNode>> children;
    double value;
    U64 checkPathMusk;
    // TEMP (debugging purposes only)
    public:
    // Board* getBoard(); // TEMP
    bool moveListEmpty(); // TEMP
    BoardNode(unique_ptr<Board> board, int lastDoublePawnMoveIndex, CastleStatus castleStatus, unordered_map<int, U64> opponentPins);
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
    vector<unique_ptr<BoardNode>>& getChildren();
    ~BoardNode();
    void clearMoves();
    bool containsMove(int fromSquare, int toSquare);
    friend void branchToChild(unique_ptr<BoardNode>& boardNode, size_t index);
    void setValue(double val);
    double getValue() const;
    multimap<double, unique_ptr<Move>, greater<double>>& getMoves();
};
#endif
