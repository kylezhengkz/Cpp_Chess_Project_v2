#include "cpu.h"
CPU::CPU(Colour colour) : Player{colour} {};

int CPU::prevDepth = -1;

double const CPU::negativeInfinity = -9999;
double const CPU::positiveInfinity = 9999;
const int CPU::maxTimeSeconds = 10;

void CPU::pickMove(unique_ptr<BoardNode> &pos) {
    if (colour == Colour::WHITE) {
        cout << "WHITE TO MOVE:" << endl;
    } else {
        cout << "BLACK TO MOVE:" << endl;
    }
    iterativeDeepening(pos);

    // move counter
    /*
     int totalMoves1 = 0;
     countTotalPossibleMoves(pos, 1, true, totalMoves1);
     cout << totalMoves1 << endl;

     int totalMoves2 = 0;
     countTotalPossibleMoves(pos, 2, true, totalMoves2);
     cout << totalMoves2 - totalMoves1  << endl;

     int totalMoves3 = 0;
     countTotalPossibleMoves(pos, 3, true, totalMoves3);
     cout << totalMoves3 - totalMoves2 << endl;

     int totalMoves4 = 0;
     countTotalPossibleMoves(pos, 4, true, totalMoves4);
     cout << totalMoves4 - totalMoves3 << endl;
     */
}

void CPU::iterativeDeepening(unique_ptr<BoardNode> &pos) {
    startTime = high_resolution_clock::now();
    int maxDepth = numeric_limits<int>::max();
    cout << "Begin iterative deepening search" << endl;
    int startDepth;
    if (prevDepth == -1) {
        startDepth = 1;
    } else {
        startDepth = prevDepth - 1;
    }
    for (int depth = startDepth; depth < maxDepth; depth++) {
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
            prevDepth = depth;
            break;
        }
        cout << "Finished searching depth " << depth << endl;

        if (pos->getValue() == positiveInfinity) {
            cout << "Forced checkmate found for white" << endl;
            break;
        } else if (pos->getValue() == negativeInfinity) {
            cout << "Forced checkmate found for black" << endl;
            break;
        }
    }
    cout << "End iterative deepening search" << endl;

    /*
    cout << "Printing leftmost path" << endl;
    unique_ptr<BoardNode>& root = pos;
    unique_ptr<BoardNode>& getPosition = pos;
    Colour switchColour = colour;
    cout << "Evaluation: " << root->getValue() << endl;
    while (getPosition->getChildren().size() > 0) {
        if (switchColour == Colour::WHITE) {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode> &lhs, const unique_ptr<BoardNode> &rhs) { return lhs->getValue() > rhs->getValue(); });
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode> &lhs, const unique_ptr<BoardNode> &rhs) { return lhs->getValue() < rhs->getValue(); });
        }

        branchToChild(getPosition, 0);
        getPosition->printBoardOnly(cout);
        switchColour = !switchColour;
    }
    pos = move(root);
    */

    branchToChild(pos, 0);
}

double CPU::quiescenceSearch(unique_ptr<BoardNode> &pos, double alpha, double beta, bool maximizingPlayer) {
    double staticEval;
    if (maximizingPlayer) {
        staticEval = pos->staticEval(Colour::WHITE);
    } else {
        staticEval = pos->staticEval(Colour::BLACK);
    }

    if (staticEval != 10000 && staticEval != 20000) {
        pos->setValue(staticEval);
        return staticEval;
    } else if (staticEval == 20000) {
        if (maximizingPlayer) {
            return negativeInfinity;
        } else {
            return positiveInfinity;
        }
    }

    if (maximizingPlayer) {
        double maxEval = negativeInfinity;
        if (pos->getChildren().size() == 0) {
            int index = -1;
            do {
                pos->addPredictedBestMove(Colour::WHITE);
                index++;
                double eval = quiescenceSearch(pos->getChildren()[index], alpha, beta, false);
                maxEval = max(maxEval, eval);
                alpha = max(alpha, eval);
                if (beta <= alpha) {
                    break;
                }
            } while (!pos->moveListEmpty() && pos->isWorthChecking(2.4));
            if (index == -1) {
                return negativeInfinity;
            }
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode> &lhs, const unique_ptr<BoardNode> &rhs) { return lhs->getValue() > rhs->getValue(); });
            for (int i = 0; i < (pos->getChildren().size() + pos->getMoves().size()); i++) {
                if (i >= pos->getChildren().size()) {
                    if (pos->isWorthChecking(2.4)) {
                        pos->addPredictedBestMove(Colour::WHITE);
                    } else {
                        break;
                    }
                }
                unique_ptr<BoardNode> &child = pos->getChildren()[i];
                double eval = quiescenceSearch(child, alpha, beta, false);
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
            int index = -1;
            do {
                pos->addPredictedBestMove(Colour::BLACK);
                index++;
                double eval = quiescenceSearch(pos->getChildren()[index], alpha, beta, true);
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                if (beta <= alpha) {
                    break;
                }
            } while (!pos->moveListEmpty() && pos->isWorthChecking(2.4));
            if (index == -1) {
                return positiveInfinity;
            }
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode> &lhs, const unique_ptr<BoardNode> &rhs) { return lhs->getValue() < rhs->getValue(); });
            for (int i = 0; i < (pos->getChildren().size() + pos->getMoves().size()); i++) {
                if (i >= pos->getChildren().size()) {
                    if (pos->isWorthChecking(2.4)) {
                        pos->addPredictedBestMove(Colour::BLACK);
                    } else {
                        break;
                    }
                }
                unique_ptr<BoardNode> &child = pos->getChildren()[i];
                double eval = quiescenceSearch(child, alpha, beta, true);
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

double CPU::alphaBetaPruning(unique_ptr<BoardNode> &pos, int depth, double alpha, double beta, bool maximizingPlayer) {
    auto currentTime = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(currentTime - startTime);
    if (duration.count() >= maxTimeSeconds) {  // time is up! stop the search
        if (depth == 0) {
            double staticEval = quiescenceSearch(pos, alpha, beta, maximizingPlayer);
            pos->setValue(staticEval);
            return staticEval;
        } else {
            return pos->getValue();
        }
    }

    if (depth == 0) {
        double staticEval = quiescenceSearch(pos, alpha, beta, maximizingPlayer);
        pos->setValue(staticEval);
        return staticEval;
    }

    if (maximizingPlayer) {
        double maxEval = negativeInfinity;
        if (pos->getChildren().size() == 0) {
            pos->generateMoves(Colour::WHITE, false);
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
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode> &lhs, const unique_ptr<BoardNode> &rhs) { return lhs->getValue() > rhs->getValue(); });
            for (int i = 0; i < (pos->getChildren().size() + pos->getMoves().size()); i++) {
                if (i >= pos->getChildren().size()) {
                    pos->addPredictedBestMove(Colour::WHITE);
                }
                unique_ptr<BoardNode> &child = pos->getChildren()[i];
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
            pos->generateMoves(Colour::BLACK, false);
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
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode> &lhs, const unique_ptr<BoardNode> &rhs) { return lhs->getValue() < rhs->getValue(); });
            for (int i = 0; i < (pos->getChildren().size() + pos->getMoves().size()); i++) {
                if (i >= pos->getChildren().size()) {
                    pos->addPredictedBestMove(Colour::BLACK);
                }
                unique_ptr<BoardNode> &child = pos->getChildren()[i];
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

void CPU::countTotalPossibleMoves(unique_ptr<BoardNode> &pos, int depth, bool maximizingPlayer, int &totalMoves) {
    if (depth == 0) {
        return;
    }

    if (maximizingPlayer) {
        pos->generateMoves(Colour::WHITE, false);
        int index = -1;
        while (!pos->moveListEmpty()) {
            pos->addPredictedBestMove(Colour::WHITE);
            index++;
            totalMoves++;
            countTotalPossibleMoves(pos->getChildren()[index], depth - 1, false, totalMoves);
        }
    } else {
        pos->generateMoves(Colour::BLACK, false);
        int index = -1;
        while (!pos->moveListEmpty()) {
            pos->addPredictedBestMove(Colour::BLACK);
            index++;
            totalMoves++;
            countTotalPossibleMoves(pos->getChildren()[index], depth - 1, true, totalMoves);
        }
    }
}
