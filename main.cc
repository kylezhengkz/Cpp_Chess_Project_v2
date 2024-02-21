#include "board.h"
#include "lookupTable.h"
#include "startPosition.h"
#include "cpu.h"
#include "user.h"
#include "constants.h"
using namespace std;
using namespace MaskUtils;
class Player;
Player* choosePlayerType(Colour colour) {
    cout << "Choose players as follows: (user OR cpu) (user OR cpu)" << endl;
    string player;
    cin >> player;
    if (player == "user") {
        return new User(colour);
    } else {
        return new CPU(colour);
    }
}

int main() {
    LookupTable::setRayMusks();
    LookupTable::setMusks();
    LookupTable::mapBlockerKeys();
    unique_ptr<Board> board{new Board()};
    unique_ptr<BoardNode> currentPosition = StartPosition::manualSetup(board);
    // unique_ptr<BoardNode> currentPosition = StartPosition::defaultPosition(board);
    // Player* player1 = choosePlayerType(Colour::WHITE);
    // player1->pickMove(currentPosition);
    Player* player1 = choosePlayerType(Colour::WHITE);
    Player* player2 = choosePlayerType(Colour::BLACK);
    int maxTurns = numeric_limits<int>::max();
    for (int i = 0; i < maxTurns; i++) {
        while (true) {
            try {
                player1->pickMove(currentPosition);
                break;
            } catch (const InvalidUserMove& e) {
                cout << e.what() << endl;
                continue;
            }
        }
        cout << ((player1->getColour() == Colour::WHITE)? "WHITE'S MOVE:" : "BLACK'S MOVE:") << endl;
        currentPosition->printBoardOnly(cout);
        while (true) {
            try {
                player2->pickMove(currentPosition);
                break;
            } catch (const InvalidUserMove& e) {
                continue;
            }
        }
        cout << ((player2->getColour() == Colour::WHITE)? "WHITE'S MOVE:" : "BLACK'S MOVE:") << endl;
        currentPosition->printBoardOnly(cout);
    }
   LookupTable::cleanup();
};
