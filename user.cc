#include "user.h"
User::User(Colour colour) : Player{colour} {}

int strToIndex(string squareStr) {
    int squareIndex = 0;
    squareIndex += (squareStr[0] - 'a');
    squareIndex += ((squareStr[1] - '0') - 1) * 8;
    return squareIndex;
}

void User::pickMove(unique_ptr<BoardNode>& pos) {
    if (colour == Colour::WHITE) {
        cout << "WHITE TO MOVE:" << endl;
    } else {
        cout << "BLACK TO MOVE:" << endl;
    }

    cout << "Input command as follows: (move OR list OR display OR printMoveGeneration)" << endl;
    string command;
    cin >> command;
    while (command != "move") {
        if (command == "list") {
            pos->generateMoves(colour, false);
            pos->printChildrenMoveNotation(cout);
            pos->clearMoves();
        } else if (command == "display") {
            pos->printBoardOnly(cout);
        } else if (command == "printMoveGeneration") {
            pos->generateMoves(colour, true);
            pos->clearMoves();
        }
        cout << "Input command as follows: (move OR list OR display OR printMoveGeneration)" << endl;
        cin >> command;
    }

    string fromSquare;
    cin >> fromSquare;
    string toSquare;
    cin >> toSquare;
    pos->generateMoves(colour, false);
    int fromSquareInt = strToIndex(fromSquare);
    int toSquareInt = strToIndex(toSquare);
    if (fromSquareInt >= 64 || fromSquareInt < 0) {
        throw logic_error("Invalid from square");
    }

    if (toSquareInt >= 64 || toSquareInt < 0) {
        throw logic_error("Invalid to square");
    }

    while (!pos->moveListEmpty()) {
        pos->addPredictedBestMove(Colour::WHITE);
    }

    size_t childIndex = pos->branchToChildInput(fromSquareInt, toSquareInt, colour);
    if (childIndex == -1) {
        throw logic_error("Invalid move");
    } else {
        branchToChild(pos, childIndex);
    }
}
