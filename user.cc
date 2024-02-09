#include "user.h"
User::User(Colour colour): Player{colour} {}

int strToIndex(string squareStr) {
    int squareIndex = 0;
    squareIndex += (squareStr[0] - 'a');
    squareIndex += ((squareStr[1] - '0') - 1) * 8;
    return squareIndex;
}

void User::pickMove(BoardNode*& pos) {
    if (colour == Colour::WHITE) {
        cout << "WHITE TO MOVE:" << endl;
    } else {
        cout << "BLACK TO MOVE:" << endl;
    }

    cout << "Input command as follows: (move OR list)" << endl;
    string command;
    cin >> command;
    while (command != "move") {
        if (command == "list") {
            pos->generateMoves(colour);
            pos->printChildren(cout);
            pos->deleteChildren();
        }
        cout << "Input command as follows: (move OR list)" << endl;
        cin >> command;
    }

    cout << "Input move as follows: (square in lower case) (square in lower case)" << endl;
    string squareOne;
    string squareTwo;
    cin >> squareOne;
    cin >> squareTwo;
    int fromSquareIndex = strToIndex(squareOne);
    int toSquareIndex = strToIndex(squareTwo);
    if (!getBit(pos->getColourPieces(colour), fromSquareIndex) // from square index does not contain your piece
    || getBit(pos->getColourPieces(colour), toSquareIndex)) { // to square is occupied by your own piece
        throw InvalidUserMove();
    }

    int branchChildIndex = -1;
    pos->generateMoves(colour);
    for (int i = 0; i < pos->getChildren().size(); i++) {
        if (!getBit(pos->getChildren()[i]->getColourPieces(colour), fromSquareIndex) // from square index in new position does not contain your piece
        && getBit(pos->getChildren()[i]->getColourPieces(colour), toSquareIndex)) { // to square contains your piece
            branchChildIndex = i;
        }
    }

    if (branchChildIndex == -1) {
        throw InvalidUserMove();
    } else {
        branchToChild(pos, branchChildIndex);
    }
}
