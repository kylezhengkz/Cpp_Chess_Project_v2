#include "cpu.h"
CPU::CPU(Colour colour): Player{colour} {};

double const CPU::negativeInfinity = -9999;
double const CPU::positiveInfinity = 9999;
const int CPU::maxTimeSeconds = 10;

int getRandomNumber(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, n);
    return distrib(gen);
}

void CPU::pickMove(BoardNode*& pos) {
    if (colour == Colour::WHITE) {
        cout << "WHITE TO MOVE:" << endl;
    } else {
        cout << "BLACK TO MOVE:" << endl;
    }
    /*
    // pick best predicted move
    pos->generateMoves(colour);
    pos->printChildrenMoveNotation(cout);
    pos->addPredictedBestMove(colour);
    branchToChild(pos, 0);
    */
    iterativeDeepening(pos);
}

void CPU::iterativeDeepening(BoardNode*& pos) {
    startTime = high_resolution_clock::now();
    for (int depth = 1; depth < numeric_limits<int>::max(); depth++) {
        cout << "Searching depth " << depth << endl;
        if (colour == Colour::WHITE) {
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, true);
        } else {
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, false);
        }
        auto currentTime = high_resolution_clock::now();
        auto timeElasped = (duration_cast<seconds>(currentTime - startTime)).count();
        cout << "Time elasped: " << timeElasped << endl;
        if (timeElasped >= maxTimeSeconds) {
            cout << "Halted search at depth " << depth << endl;
            break;
        }
        cout << "Finished searching depth " << depth << endl;
    }
    branchToChild(pos, 0);
}

double CPU::alphaBetaPruning(BoardNode* pos, int depth, double alpha, double beta, bool maximizingPlayer) {
    auto currentTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(currentTime - startTime);
    if (duration.count() >= maxTimeSeconds) { // time is up! stop the search
        if (depth == 0) {
            double staticEval = pos->staticEval();
            pos->setValue(staticEval);
            return staticEval;
        } else {
            return pos->getValue();
        }
    }

    if (depth == 0) {
        double staticEval = pos->staticEval();
        pos->setValue(staticEval);
        return staticEval;
    }

    if (maximizingPlayer) {
        double maxEval = negativeInfinity;
        if (pos->getChildren().size() == 0) {
            pos->generateMoves(Colour::WHITE);
            int index = -1;
            while (!pos->moveListEmpty()) {
                pos->addPredictedBestMove(Colour::WHITE);
                index++;
                double eval = alphaBetaPruning(pos->getChildren()[index], depth - 1, alpha, beta, false);
                maxEval = max(maxEval, eval);
                alpha = max(alpha, eval);
                if (beta <= alpha) {
                    break;
                }
            }
        } else {
            vector<BoardNode*> vec = pos->getChildren();
            sort(vec.begin(), vec.end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() > rhs->getValue();
            });
            for (BoardNode* child : pos->getChildren()) {
                double eval = alphaBetaPruning(child, depth - 1, alpha, beta, false);
                maxEval = max(maxEval, eval);
                alpha = max(alpha, eval);
                if (beta <= alpha) {
                    break;
                }
            }
        }
        pos->setValue(maxEval);
        return maxEval;
    } else {
        double minEval = positiveInfinity;
        if (pos->getChildren().size() == 0) {
            pos->generateMoves(Colour::BLACK);
            int index = -1;
            while (!pos->moveListEmpty()) {
                pos->addPredictedBestMove(Colour::BLACK);
                index++;
                double eval = alphaBetaPruning(pos->getChildren()[index], depth - 1, alpha, beta, true);
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                if (beta <= alpha) {
                    break;
                }
            }
        } else {
            vector<BoardNode*> vec = pos->getChildren();
            sort(vec.begin(), vec.end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() < rhs->getValue();
            });
            for (BoardNode* child : pos->getChildren()) {
                double eval = alphaBetaPruning(child, depth - 1, alpha, beta, true);
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                if (beta <= alpha) {
                    break;
                }
            }
        }
        pos->setValue(minEval);
        return minEval;
    }
}

void CPU::quiescenceSearch() {
    
}
