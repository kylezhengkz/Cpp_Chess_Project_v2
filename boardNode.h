#ifndef BOARDNODE_H
#define BOARDNODE_H
#include <iterator>
#include <map>
#include <random>
#include <vector>

#include "board.h"
#include "castleStatus.h"
#include "lookupTable.h"
#include "move.h"
class BoardNode {
    unique_ptr<Board> board;
    int lastDoublePawnMoveIndex;
    CastleStatus castleStatus;
    vector<unique_ptr<Move>> moves;
    vector<unique_ptr<BoardNode>> children;
    double value;
    U64 checkPathMusk;
    BoardNode* parent;

   public:
    bool moveListEmpty();
    BoardNode(unique_ptr<Board> board, int lastDoublePawnMoveIndex, CastleStatus castleStatus, BoardNode* parent);
    double staticEval(Colour colour);
    void checkPinsAndChecks(Colour colour, bool &check, bool &doubleCheck, U64 &kingLegalMoves, int &kingSquare, unordered_map<int, U64> &pins, U64 teamPieces, U64 opponentPieces, bool print);
    void generateOpponentChecksAndUnsafeMusk(Colour myColour, U64 &unsafeMusk, U64 &diagonalChecks, U64 &straightChecks, U64 &knightChecks, U64 &pawnChecks, U64 teamPieces, U64 opponentPieces, bool print);
    void generateMoves(Colour colour, bool print);
    void addPredictedBestMove(Colour colour);
    ostream &printBoardOnly(ostream &out);
    ostream &printChildrenValues(ostream &out);
    ostream &printChildrenMoveNotation(ostream &out);
    ostream &printChildrenTree(ostream &out);
    ostream &printChildrenValue(ostream &out);
    vector<unique_ptr<BoardNode>> &getChildren();
    ~BoardNode();
    void clearMoves();
    bool containsMove(int fromSquare, int toSquare);
    friend void branchToChild(unique_ptr<BoardNode> &boardNode, size_t index);
    size_t branchToChildInput(int fromSquare, int toSquare, Colour colour);
    void setValue(double val);
    double getValue() const;
    vector<unique_ptr<Move>> &getMoves();
};
#endif
