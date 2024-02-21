#include "cpu.h"
CPU::CPU(Colour colour): Player{colour} {};

double const CPU::negativeInfinity = -9999;
double const CPU::positiveInfinity = 9999;
const int CPU::maxTimeSeconds = 10;

void CPU::pickMove(unique_ptr<BoardNode>& pos) {
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

void CPU::iterativeDeepening(unique_ptr<BoardNode>& pos) {
    startTime = high_resolution_clock::now();
    int maxDepth = numeric_limits<int>::max();
    cout << "Begin iterative deepening search" << endl;
    for (int depth = 1; depth < maxDepth; depth++) {
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

        if (pos->getValue() == positiveInfinity) {
            cout << "Forced checkmate found for white" << endl;
            break;
        } else if (pos->getValue() == negativeInfinity) {
            cout << "Forced checkmate found for black" << endl;
            break;
        }
    }
    cout << "End iterative deepening search" << endl;
    branchToChild(pos, 0);
    
    /*
    cout << "Evaluation: " << pos->getValue() << endl;
    pos->printBoardOnly(cout);
    cout << "Printing leftmost path" << endl;
    Colour switchColour = colour;
    while (pos->getChildren().size() > 0) {
        if (switchColour == Colour::WHITE) {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode>& lhs, const unique_ptr<BoardNode>& rhs) {
                return lhs->getValue() > rhs->getValue();
            });
        } else {
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode>& lhs, const unique_ptr<BoardNode>& rhs) {
                return lhs->getValue() < rhs->getValue();
            });
        }
        pos->printBoardOnly(cout);

        if (switchColour == Colour::WHITE) {
            switchColour = Colour::BLACK;
        } else {
            switchColour = Colour::WHITE;
        }
        branchToChild(pos, 0);
    }
    */
}

double CPU::alphaBetaPruning(unique_ptr<BoardNode>& pos, int depth, double alpha, double beta, bool maximizingPlayer) {
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
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode>& lhs, const unique_ptr<BoardNode>& rhs) {
                return lhs->getValue() > rhs->getValue();
            });
            for (int i = 0; i < (pos->getChildren().size() + pos->getMoves().size()); i++) {
                if (i >= pos->getChildren().size()) {
                    pos->addPredictedBestMove(Colour::WHITE);
                }
                unique_ptr<BoardNode>& child = pos->getChildren()[i];
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
            sort(pos->getChildren().begin(), pos->getChildren().end(), [](const unique_ptr<BoardNode>& lhs, const unique_ptr<BoardNode>& rhs) {
                return lhs->getValue() < rhs->getValue();
            });
            for (int i = 0; i < (pos->getChildren().size() + pos->getMoves().size()); i++) {
                if (i >= pos->getChildren().size()) {
                    pos->addPredictedBestMove(Colour::BLACK);
                }
                unique_ptr<BoardNode>& child = pos->getChildren()[i];
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

void CPU::countTotalPossibleMoves(unique_ptr<BoardNode>& pos, int depth, bool maximizingPlayer, int& totalMoves) {
    if (depth == 0) {
        return;
    }

    if (maximizingPlayer) {
        pos->generateMoves(Colour::WHITE);
        int index = -1;
        while (!pos->moveListEmpty()) {
            pos->addPredictedBestMove(Colour::WHITE);
            index++;
            totalMoves++;
            countTotalPossibleMoves(pos->getChildren()[index], depth - 1, false, totalMoves);
        }
    } else {
        pos->generateMoves(Colour::BLACK);
        int index = -1;
        while (!pos->moveListEmpty()) {
            pos->addPredictedBestMove(Colour::BLACK);
            index++;
            totalMoves++;
            countTotalPossibleMoves(pos->getChildren()[index], depth - 1, true, totalMoves);
        }
    }
}
