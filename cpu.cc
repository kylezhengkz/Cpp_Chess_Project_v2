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
    pos->generateMoves(colour);
    pos->printChildrenMoveNotation(cout);
    pos->addPredictedBestMove(colour);
    branchToChild(pos, 0);
    // iterativeDeepening(pos);
}

void CPU::iterativeDeepening(BoardNode*& pos) {
    startTime = high_resolution_clock::now();
    for (int depth = 1; depth < numeric_limits<int>::max(); depth++) {
        if (colour == Colour::WHITE) {
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, true);
        } else {
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, false);
        }
        auto currentTime = high_resolution_clock::now();
        if ((duration_cast<seconds>(currentTime - startTime)).count() >= maxTimeSeconds) {
            break;
        }
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
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() > rhs->getValue();
            });
        }

        for (size_t i = 0; i < pos->getChildren().size(); i++) {
            double eval = alphaBetaPruning(pos->getChildren()[i], depth - 1, alpha, beta, false);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) {
                break;
            }
        }
        pos->setValue(maxEval);
        return maxEval;

        throw logic_error("Logically unreachable code has been reached");
        return negativeInfinity;
    } else {
        double minEval = positiveInfinity;
        if (pos->getChildren().size() == 0) {
            pos->generateMoves(Colour::BLACK);
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() < rhs->getValue();
            });
        }

        for (size_t i = 0; i < pos->getChildren().size(); i++) {
            double eval = alphaBetaPruning(pos->getChildren()[i], depth - 1, alpha, beta, true);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) {
                break;
            }
        }
        pos->setValue(minEval);
        return minEval;

        throw logic_error("Logically unreachable code has been reached");
        return positiveInfinity;
    }
}
