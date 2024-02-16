#include "cpu.h"
CPU::CPU(Colour colour): Player{colour} {};

double const CPU::negativeInfinity = -9999;
double const CPU::positiveInfinity = 9999;
const int CPU::maxTimeSeconds = 10;

void CPU::pickMove(BoardNode*& pos) {
    if (colour == Colour::WHITE) {
        cout << "WHITE TO MOVE:" << endl;
    } else {
        cout << "BLACK TO MOVE:" << endl;
    }
    // pick best predicted move
    /*
    pos->generateMoves(colour);
    pos->printChildrenMoveNotation(cout);
    pos->addPredictedBestMove(colour);
    branchToChild(pos, 0);
    */
    iterativeDeepening(pos);
    // pos->printChildrenTree(cout);
}

void CPU::iterativeDeepening(BoardNode*& pos) {
    startTime = high_resolution_clock::now();
    // int maxDepth = numeric_limits<int>::max();
    int maxDepth = 256;
    // cout << "Begin iterative deepening search" << endl;
    for (int depth = 1; depth < maxDepth; depth++) {
        // cout << "Searching depth " << depth << endl;
        if (colour == Colour::WHITE) {
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, true);
        } else {
            alphaBetaPruning(pos, depth, negativeInfinity, positiveInfinity, false);
        }
        auto currentTime = high_resolution_clock::now();
        auto timeElasped = (duration_cast<seconds>(currentTime - startTime)).count();
        // cout << "Time elasped: " << timeElasped << endl;
        if (timeElasped >= maxTimeSeconds) {
            // cout << "Halted search at depth " << depth << endl;
            break;
        }
        // cout << "Finished searching depth " << depth << endl;
    }
    // cout << "End iterative deepening search" << endl;
    
    cout << "View leftmost path" << endl;
    BoardNode* currentNode = pos;
    cout << "value of position below: " << pos->getValue() << endl;
    currentNode->printBoardOnly(cout);
    /*
    Colour switchColour = colour;
    while (currentNode->getChildren().size() != 0) {
        if (switchColour == Colour::WHITE) {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() > rhs->getValue();
            });
            switchColour = Colour::BLACK;
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() < rhs->getValue();
            });
            switchColour = Colour::WHITE;
        }
        currentNode = currentNode->getChildren()[0];
        cout << "value of position below: " << pos->getValue() << endl;
        currentNode->printBoardOnly(cout);
    }
    */
   if (colour == Colour::WHITE) {
        cout << "before" << endl;
        pos->printChildrenValues(cout);
        sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
            return lhs->getValue() > rhs->getValue();
        });
        cout << "after" << endl;
        pos->printChildrenValues(cout);
    } else {
        cout << "before" << endl;
        pos->printChildrenValues(cout);
        sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
            return lhs->getValue() < rhs->getValue();
        });
        cout << "after" << endl;
        pos->printChildrenValues(cout);
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
            if (index == -1) {
                return negativeInfinity;
            }
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
                return lhs->getValue() > rhs->getValue();
            });
            for (BoardNode* child : pos->getChildren()) {
                if (pos->getMoves().begin()->first > child->getValue()) {
                    pos->addPredictedBestMove(Colour::WHITE);
                }
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
            if (index == -1) {
                return positiveInfinity;
            }
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const BoardNode* lhs, const BoardNode* rhs) {
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
