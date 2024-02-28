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

    cout << "Input command as follows: (move OR list OR display)" << endl;
    string command;
    cin >> command;
    while (command != "move") {
        if (command == "list") {
            pos->generateMoves(colour);
            pos->printChildrenMoveNotation(cout);
            pos->clearMoves();
        } else if (command == "display") {
            pos->printBoardOnly(cout);
        }
        cout << "Input command as follows: (move OR list OR display)" << endl;
        cin >> command;
    }
}
