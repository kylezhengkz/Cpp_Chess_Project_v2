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
}

void CPU::iterativeDeepening(BoardNode*& pos) {
    // int temp = numeric_limits<int>::max();
    int temp = 2;
    for (int depth = 1; depth < temp; depth++) {
        if (colour == Colour::WHITE) {
            startTime = high_resolution_clock::now();
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, true);
        } else {
            startTime = high_resolution_clock::now();
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, false);
        }
    }
    for (size_t i = 1; i < pos->getChildren().size(); i++) {
        delete pos->getChildren()[i];
    }
    pos->deleteBoard();
    pos = pos->getChildren()[0];
    pos->getChildren().clear();
}

double CPU::alphaBetaPruning(BoardNode* pos, int depth, double alpha, double beta, bool maximizingPlayer) {
    auto currentTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(currentTime - startTime);
    if (duration.count() >= maxTimeSeconds) { // time is up! stop the search
        return pos->staticEval();
    }

    if (depth == 0) {
        return pos->staticEval();
    }

    if (maximizingPlayer) {
        double maxEval = negativeInfinity;
        bool newMoves = false;
        if (pos->getChildren().size() == 0) {
            pos->generateMoves(Colour::WHITE);
            newMoves = true;
        }
        /*
        if (!newMoves) { // order move
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() < rhs->getValue();
            });
        }
        */
        for (size_t i = 0; i < pos->getChildren().size(); i++) {
            double eval = alphaBetaPruning(pos->getChildren()[i], depth - 1, alpha, beta, false);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) {
                break;
            }
            return maxEval;
        }
        return negativeInfinity;
    } else {
        double minEval = positiveInfinity;
        bool newMoves = false;
        if (pos->getChildren().size() == 0) {
            pos->generateMoves(Colour::BLACK);
            newMoves = true;
        }
        /*
        if (!newMoves) { // order move
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() < rhs->getValue();
            });
        }
        */
        for (size_t i = 0; i < pos->getChildren().size(); i++) {
            double eval = alphaBetaPruning(pos->getChildren()[i], depth - 1, alpha, beta, true);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) {
                break;
            }
            return minEval;
        }
        return positiveInfinity;
    }
}
