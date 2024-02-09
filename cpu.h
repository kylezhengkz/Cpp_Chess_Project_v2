#ifndef CPU_H
#define CPU_H
#include "boardNode.h"
#include "constants.h"
#include <algorithm>
#include <limits>
#include <chrono>
#include <thread>
#include "player.h"
using namespace chrono;
class CPU: public Player {
    public:
    CPU(Colour colour);
    void generateMoves();
    void pickMove(BoardNode*& pos) override; // DEBUGGING PURPOSES
    void iterativeDeepening(BoardNode*& pos);
    double alphaBetaPruning(BoardNode* pos, int depth, double alpha, double beta, bool maximizingPlayer);
    private:
    static const double negativeInfinity;
    static const double positiveInfinity;
    static const double prunedWhite;
    static const double prunedBlack;
    static const int maxTimeSeconds;
    high_resolution_clock::time_point startTime;
};
#endif
