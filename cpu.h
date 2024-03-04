#ifndef CPU_H
#define CPU_H
#include <algorithm>
#include <chrono>
#include <limits>

#include "boardNode.h"
#include "constants.h"
#include "player.h"
using namespace chrono;
class CPU : public Player {
    static int prevDepth;
   public:
    CPU(Colour colour);
    void generateMoves();
    void pickMove(unique_ptr<BoardNode> &pos) override;
    void iterativeDeepening(unique_ptr<BoardNode> &pos);
    double quiescenceSearch(unique_ptr<BoardNode> &pos, double alpha, double beta, bool maximizingPlayer);
    double alphaBetaPruning(unique_ptr<BoardNode> &pos, int depth, double alpha, double beta, bool maximizingPlayer);
    void countTotalPossibleMoves(unique_ptr<BoardNode> &pos, int depth, bool maximizingPlayer, int &totalMoves);

   private:
    static const double negativeInfinity;
    static const double positiveInfinity;
    static const int maxTimeSeconds;
    high_resolution_clock::time_point startTime;
};
#endif
