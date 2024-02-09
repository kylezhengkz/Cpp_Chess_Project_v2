#include "boardNode.h"
string generateRandomID() { // TEMP
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> indexDist(0, charset.size() - 1);
    std::string id;
    for (int i = 0; i < 8; ++i) {
        id += charset[indexDist(gen)];
    }
    return id;
}

BoardNode::BoardNode(Board* board, double value, int lastDoublePawnMoveIndex, CastleStatus castleStatus, unordered_map<int, U64> opponentPins):
board{board}, value{value}, lastDoublePawnMoveIndex{lastDoublePawnMoveIndex}, castleStatus{castleStatus}, opponentPins{opponentPins} {
    id = generateRandomID();
};

U64 BoardNode::getColourPieces(Colour colour) {
    if (colour == Colour::WHITE) {
        return board->whitePieces;
    } else {
        return board->blackPieces;
    }
}

double BoardNode::staticEval() {
    double eval = 0;
    eval += __builtin_popcountll(board->whitePawns) * 1;
    eval += __builtin_popcountll(board->whiteKnights) * 3;
    eval += __builtin_popcountll(board->whiteBishops) * 3.05;
    eval += __builtin_popcountll(board->whiteRooks) * 5;
    eval += __builtin_popcountll(board->whiteQueens) * 9;
    eval -= __builtin_popcountll(board->blackPawns) * 1;
    eval -= __builtin_popcountll(board->blackKnights) * 3;
    eval -= __builtin_popcountll(board->blackBishops) * 3.05;
    eval -= __builtin_popcountll(board->blackRooks) * 5;
    eval -= __builtin_popcountll(board->blackQueens) * 9;
    return eval;
}

void BoardNode::searchLegalMusks(Colour colour, unordered_map<int, U64>& pins, bool& check, U64& checkPath, bool& doubleCheck) {
    int kingSquare;
    U64 oppositionPawns;
    U64 oppositionKnights;
    U64 oppositionBishops;
    U64 oppositionRooks;
    U64 oppositionQueens;
    U64 teamPieces;
    U64 oppositionPieces;
    if (colour == Colour::WHITE) {
        kingSquare = getLSB(board->whiteKing);
        oppositionPawns = board->blackPawns;
        oppositionKnights = board->blackKnights;
        oppositionBishops = board->blackBishops;
        oppositionRooks = board->blackRooks;
        oppositionQueens = board->blackQueens;
        teamPieces = board->whitePieces;
    } else {
        kingSquare = getLSB(board->blackKing);
        oppositionPawns = board->whitePawns;
        oppositionKnights = board->whiteKnights;
        oppositionBishops = board->whiteBishops;
        oppositionRooks = board->whiteRooks;
        oppositionQueens = board->whiteQueens;
        oppositionPieces = board->blackPieces;
    }

    // look for pawn checks
    U64 checkMusk = LookupTable::lookupPawnControlMusk(kingSquare, colour);
    if ((checkMusk & oppositionPawns) != 0) {
        check = true;
        checkPath = checkMusk & oppositionPawns;
    }

    // look for knight checks
    checkMusk = LookupTable::lookupMusk(kingSquare, Piece::KNIGHT);
    if (!check && (checkMusk & oppositionKnights) != 0) {
        check = true;
        checkPath = checkMusk & oppositionKnights;
    }
    
    // quick way to ensure that check is possible
    U64 potentialCheckPaths = LookupTable::lookupMusk(kingSquare, Piece::QUEEN);
    if ((potentialCheckPaths & (oppositionBishops | oppositionRooks | oppositionQueens)) == 0) {
        return;
    }

    // search each check ray direction one-by-one
    vector<int> increasingRays = {VERTICAL, POSITIVE_DIAGONAL, NEGATIVE_DIAGONAL, HORIZONTAL};
    for (int direction : increasingRays) {
        U64 ray = LookupTable::lookupRayMusk(kingSquare, direction);
        U64 oppositionAttackers;
        if (isDiagonal(direction)) {
            oppositionAttackers = oppositionBishops | oppositionQueens;
        } else {
            oppositionAttackers = oppositionRooks | oppositionQueens;
        }

        if ((ray & oppositionAttackers) != 0) {
            int sourceIndex = getLSB(ray & (oppositionRooks | oppositionQueens));
            if (sourceIndex == -1) {
                continue;
            }
            U64 inBetweenRay = clearBitsGreaterThanIndex(ray, sourceIndex);
            if (inBetweenRay & oppositionPieces) { // opponent piece in the way
                continue;
            } else if ((inBetweenRay & teamPieces) == 0) { // check
                if (check) {
                    doubleCheck = true;
                    return;
                } else {
                    checkPath = inBetweenRay;
                }
            } else if (__builtin_popcountll(inBetweenRay & teamPieces) == 1) { // pin
                pins[getLSB(inBetweenRay & teamPieces)] = inBetweenRay;
            }
        }
    }

    vector<int> decreasingRays = {-VERTICAL, -POSITIVE_DIAGONAL, -NEGATIVE_DIAGONAL, -HORIZONTAL};
    for (int direction : decreasingRays) {
        U64 ray = LookupTable::lookupRayMusk(kingSquare, direction);
        U64 oppositionAttackers;
        if (isDiagonal(direction)) {
            oppositionAttackers = oppositionBishops | oppositionQueens;
        } else {
            oppositionAttackers = oppositionRooks | oppositionQueens;
        }

        if ((ray & oppositionAttackers) != 0) {
            int sourceIndex = getMSB(ray & (oppositionRooks | oppositionQueens));
            U64 inBetweenRay = clearBitsLessThanIndex(ray, sourceIndex);
            if (inBetweenRay & oppositionPieces) { // no check or pin due to opponent piece interference
                continue;
            } else if ((inBetweenRay & teamPieces) == 0) { // check
                if (check) {
                    doubleCheck = true;
                    return;
                } else {
                    checkPath = inBetweenRay & sourceIndex;
                }
            } else if (__builtin_popcountll(inBetweenRay & teamPieces) == 1) { // pin
                pins[getLSB(inBetweenRay & teamPieces)] = inBetweenRay & sourceIndex;
            }
        }
    }
}

U64 BoardNode::generateUnsafeMusk(Colour teamColour) {
    Colour oppositionColour;
    if (teamColour == Colour::WHITE) {
        oppositionColour = Colour::BLACK;
    } else {
        oppositionColour = Colour::WHITE;
    }

    U64 allPieces = board->whitePieces | board->blackPieces;
    vector<Piece> pieceGenerationOrder;
    U64 teamPieces;
    if (oppositionColour == Colour::WHITE) {
        pieceGenerationOrder = {Piece::WHITEPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        teamPieces = board->whitePieces;
    } else {
        pieceGenerationOrder = {Piece::BLACKPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        teamPieces = board->blackPieces;
    }

    U64 controlMusk = 0x0ULL;
    for (Piece piece : pieceGenerationOrder) {
        U64 existingPieces = board->getPieces(piece, oppositionColour);
        while (existingPieces != 0) {
            int initialSquare = popLSB(existingPieces);
            if (piece == Piece::WHITEPAWN || piece == Piece::BLACKPAWN) {
                controlMusk |= LookupTable::lookupPawnControlMusk(initialSquare, oppositionColour);
            } else if (piece == Piece::KNIGHT) {
                controlMusk |= LookupTable::lookupMusk(initialSquare, piece) & ~teamPieces;
            } else {
                controlMusk |= LookupTable::lookupMove(initialSquare, piece, allPieces) & ~teamPieces;
            }

            if (opponentPins.count(initialSquare) > 0) {
                controlMusk &= opponentPins[initialSquare];
            }
        }
    }
    return controlMusk;
}

/*
Algorithm:
1) Generate legal musks
- Look in each ray direction for possible checks
- If check, only enable king moves or & with check ray
- Query pinned pieces in the process, and & legal musks for those pieces
- If double check, only enable king moves

2) Generate unsafe squares and get unsafe major pieces

3) Generate legal moves reguarly
- If capture, add piece value to expected move value
- Deduct 75% of piece value for unsafe square destinations
- Increase by piece value for successful evasions
- For each king move, see if that square is safe
- Deduct 5 for king moves when castling is available

4) For each legal move, generate their destination squares (do not create new board)
- If check, add default value of 3.5 to expected move
- Add 25% of piece value for each attack (accounts for forks)

Note that:
Pinned pieces remain pinned on your turn unless you move king or you move piece in the way of pin
So you have access to opponent's pinned pieces
Make sure you add pinned piece if blocking check
*/

void BoardNode::generateMoves(Colour colour) {
    unordered_map<int, U64> pins;
    bool check = false;
    U64 checkPath;
    bool doubleCheck = false;
    searchLegalMusks(colour, pins, check, checkPath, doubleCheck);

    U64 unsafeSquares = generateUnsafeMusk(colour);

    U64 allPieces = board->whitePieces | board->blackPieces;
    vector<Piece> pieceGenerationOrder;
    U64 teamPieces;
    U64 oppositionPieces;
    if (colour == Colour::WHITE) {
        pieceGenerationOrder = {Piece::WHITEPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        teamPieces = board->whitePieces;
        oppositionPieces = board->blackPieces;
    } else {
        pieceGenerationOrder = {Piece::BLACKPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        teamPieces = board->blackPieces;
        oppositionPieces = board->whitePieces;
    }

    for (Piece piece : pieceGenerationOrder) {
        U64 existingPieces = board->getPieces(piece, colour);
        while (existingPieces != 0) {
            int initialSquare = popLSB(existingPieces);
            U64 legalMoves;
            if (piece == Piece::KNIGHT) {
                legalMoves = LookupTable::lookupMusk(initialSquare, piece) & ~teamPieces;
            } else {
                legalMoves = LookupTable::lookupMove(initialSquare, piece, allPieces) & ~teamPieces;
            }

            if (check) {
                legalMoves &= checkPath;
            }

            if (pins.count(initialSquare) > 0) {
                legalMoves &= pins[initialSquare];
            }

            while (legalMoves != 0) {
                int newSquare = popLSB(legalMoves);
                double moveVal = 0;

                int8_t flag = 0b0000;
                switch (piece) {
                    case Piece::WHITEPAWN:
                        flag = Move::pawnMove;
                        break;
                    case Piece::BLACKPAWN:
                        flag = Move::pawnMove;
                        break;
                    case Piece::KNIGHT:
                        flag = Move::knightMove;
                        break;
                    case Piece::BISHOP:
                        flag = Move::bishopMove;
                        break;
                    case Piece::ROOK:
                        flag = Move::rookMove;
                        break;
                    case Piece::QUEEN:
                        flag = Move::queenMove;
                        break;
                    default:
                        break;
                }

                int8_t captureFlag = 0b0000;

                if (getBit(oppositionPieces, newSquare)) { // if capture
                    moveVal += board->findPiece(newSquare, !colour, captureFlag);
                }

                if (getBit(unsafeSquares, newSquare)) {
                    moveVal -= board->getPieceValue(piece) * 0.75; // move to an unsafe square
                } else if (getBit(unsafeSquares, initialSquare)) {
                    moveVal += board->getPieceValue(piece); // evasions
                }

                U64 futureMuskDestinations;
                if (piece == Piece::WHITEPAWN) {
                    futureMuskDestinations = LookupTable::lookupPawnControlMusk(newSquare, colour);
                    moveVal += __builtin_popcountll(board->getPieces(Piece::WHITEPAWN, colour)) * 0.5;
                } else if (piece == Piece::BLACKPAWN) {
                    futureMuskDestinations = LookupTable::lookupPawnControlMusk(newSquare, colour);
                    moveVal += __builtin_popcountll(board->getPieces(Piece::BLACKPAWN, colour)) * 0.5;
                } else if (piece == Piece::KNIGHT) {
                    futureMuskDestinations = LookupTable::lookupMusk(newSquare, piece) & ~teamPieces;
                } else {
                    futureMuskDestinations = LookupTable::lookupMove(newSquare, piece, allPieces) & ~teamPieces;
                }

                while (futureMuskDestinations != 0) {
                    int destinationSquare = popLSB(futureMuskDestinations);
                    if (!getBit(allPieces, destinationSquare)) {
                        continue;
                    } else if (getBit(board->getPieces(Piece::KING, !colour), destinationSquare)) { // check
                        moveVal += 3.5;
                    } else if (getBit(oppositionPieces, destinationSquare)) {
                        int8_t dummy = 0b0000;
                        moveVal += board->findPiece(destinationSquare, !colour, dummy) * 0.25;
                    }
                }
            
                Move move{initialSquare, newSquare, flag, captureFlag};
                moves.emplace(moveVal, move);
            }
        }
    }

    // generate king moves below
}

void BoardNode::branchPredictedBestMove(Colour colour) {
    if (moves.size() <= 0) {
        throw logic_error("Attempted to branch but there are no moves to branch");
    }

    Board* newPosition = new Board(*board);
    
    auto bestPredictedMove = moves.rbegin()->second;
    int flag = bestPredictedMove.getFlag();

    if (colour == Colour::WHITE) {
        switch (flag) {
            case Move::noFlag:
                throw logic_error("No flag");
                break;
            case Move::castle:
                break;
            case Move::enPassant:
                break;
            case Move::pawnDoubleMove:
                break;
            case Move::queenPromotion:
                break;
            case Move::rookPromotion:
                break;
            case Move::knightPromotion:
                break;
            case Move::bishopPromotion:
                break;
            case Move::pawnMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove.getFromSquare()) | (0x1ULL << bestPredictedMove.getToSquare());
                break;
            case Move::knightMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove.getFromSquare()) | (0x1ULL << bestPredictedMove.getToSquare());
                break;
            case Move::bishopMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove.getFromSquare()) | (0x1ULL << bestPredictedMove.getToSquare());
                break;
            case Move::rookMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove.getFromSquare()) | (0x1ULL << bestPredictedMove.getToSquare());
                break;
            case Move::queenMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove.getFromSquare()) | (0x1ULL << bestPredictedMove.getToSquare());
                break;
            case Move::kingMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove.getFromSquare()) | (0x1ULL << bestPredictedMove.getToSquare());
                break;
            default:
                throw logic_error("Undefined flag");
                break;
            }
    } else {

    }
}

ostream& operator<<(ostream& out, BoardNode& boardNode) {
    out << "BOARD NODE ID: " << boardNode.id << "——————————————————————————————————————————————————————————————————————————" << endl;
    out << *(boardNode.board);
    out << "CHILDREN OF PARENT ID (SIZE: " << boardNode.children.size() << "): " << boardNode.id << endl;
    for (auto child : boardNode.children) {
        cout << *child;
    }
    out << "END CHILDREN OF PARENT ID: " << boardNode.id << endl;
    cout << "Value: " << boardNode.value << endl;
    cout << "Can white king castle right? " << ((boardNode.castleStatus.canWhiteKingCastleRight() == true) ? "Yes" : "No") << endl;
    cout << "Can white king castle right? " << ((boardNode.castleStatus.canWhiteKingCastleLeft() == true) ? "Yes" : "No") << endl;
    cout << "Can white king castle right? " << ((boardNode.castleStatus.canBlackKingCastleRight() == true) ? "Yes" : "No") << endl;
    cout << "Can white king castle right? " << ((boardNode.castleStatus.canBlackKingCastleLeft() == true) ? "Yes" : "No") << endl;
    cout << "Last Double Pawn Move Index: " << boardNode.lastDoublePawnMoveIndex << endl;
    out << "END BOARD NODE ——————————————————————————————————————————————————————————————————————————" << endl;
    return out;
}

ostream& BoardNode::printBoardOnly(ostream& out) {
    out << *(board) << endl;
    return out;
}

ostream& BoardNode::printChildren(ostream& out) {
    for (size_t i = 0; i < children.size(); i++) {
        out << "Move " << i + 1 << ":" << endl;
        children[i]->printBoardOnly(out);
    }
    return out;
}

vector<BoardNode*> BoardNode::getChildren() {
    return children;
}

void BoardNode::setNewValue(double newValue) {
    value = newValue;
}

double BoardNode::getValue() const {
    return value;
}

BoardNode::~BoardNode() {
    delete board;
    for (BoardNode* boardNodePtr : children) {
        delete boardNodePtr;
    }
}

void BoardNode::deleteBoard() {
    delete board;
}

void BoardNode::deleteChildren() {
    for (BoardNode* child : children) {
        delete child;
    }
    children.clear();
}

void branchToChild(BoardNode*& boardNode, size_t childIndex) {
    for (size_t i = 0; i < boardNode->getChildren().size(); i++) {
        if (childIndex == i) {
            continue;
        }
        delete boardNode->getChildren()[i];
    }
    boardNode->deleteBoard();
    boardNode = boardNode->getChildren()[childIndex];
    boardNode->getChildren().clear();
}
