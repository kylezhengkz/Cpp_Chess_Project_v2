#include "boardNode.h"

bool BoardNode::moveListEmpty() { return moves.empty(); }

BoardNode::BoardNode(unique_ptr<Board> board, int lastDoublePawnMoveIndex, CastleStatus castleStatus, BoardNode *parent) : board{move(board)}, lastDoublePawnMoveIndex{lastDoublePawnMoveIndex}, castleStatus{castleStatus}, parent{parent} {};

double BoardNode::staticEval(Colour colour) {
    double eval = 0;

    // material balance
    eval += __builtin_popcountll(board->whitePawns) * 1;
    eval += __builtin_popcountll(board->whiteKnights) * 3;
    eval += __builtin_popcountll(board->whiteBishops) * 3;
    eval += __builtin_popcountll(board->whiteRooks) * 5;
    eval += __builtin_popcountll(board->whiteQueens) * 9;
    eval -= __builtin_popcountll(board->blackPawns) * 1;
    eval -= __builtin_popcountll(board->blackKnights) * 3;
    eval -= __builtin_popcountll(board->blackBishops) * 3;
    eval -= __builtin_popcountll(board->blackRooks) * 5;
    eval -= __builtin_popcountll(board->blackQueens) * 9;

    generateMoves(colour, false);
    if (moves.size() == 0) {
        return 20000;
    }
    if (isWorthChecking(2.4)) {
        return 10000; // quiescenceSearch
    }

    vector<Piece> pieceGenerationOrder;
    Piece switchPiece;
    Colour oppositionColour;
    U64 opponentPawns;
    U64 switchOpponentPawns;
    if (colour == Colour::WHITE) {
        oppositionColour = Colour::BLACK;
        pieceGenerationOrder = {Piece::WHITEPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        opponentPawns = board->getPieces(Piece::BLACKPAWN, oppositionColour);
        switchPiece = Piece::BLACKPAWN;
        switchOpponentPawns = board->getPieces(Piece::WHITEPAWN, colour);
    } else {
        oppositionColour = Colour::WHITE;
        pieceGenerationOrder = {Piece::BLACKPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        opponentPawns = board->getPieces(Piece::WHITEPAWN, oppositionColour);
        switchPiece = Piece::WHITEPAWN;
        switchOpponentPawns = board->getPieces(Piece::BLACKPAWN, colour);
    }

    U64 allPieces = board->getWhitePiecesMusk() | board->getBlackPiecesMusk();

    for (Piece piece : pieceGenerationOrder) {
        U64 existingPieces = board->getPieces(piece, colour);
        while (existingPieces != 0) {
            int square = popLSB(existingPieces);
            if ((piece == Piece::WHITEPAWN || piece == Piece::BLACKPAWN)) {
                eval += LookupTable::lookupPawnPSTValue(square, opponentPawns, colour);
            } else if (piece == Piece::KNIGHT) {
                eval += LookupTable::lookupKnightPSTValue(square, allPieces);
            } else {
                eval += LookupTable::lookupPSTValue(square, piece, allPieces);
            }
        }
    }

    pieceGenerationOrder[0] = switchPiece;
    for (Piece piece : pieceGenerationOrder) {
        U64 existingPieces = board->getPieces(piece, oppositionColour);
        while (existingPieces != 0) {
            int square = popLSB(existingPieces);
            if ((piece == Piece::WHITEPAWN || piece == Piece::BLACKPAWN)) {
                eval -= LookupTable::lookupPawnPSTValue(square, switchOpponentPawns, oppositionColour);
            } else if (piece == Piece::KNIGHT) {
                eval -= LookupTable::lookupKnightPSTValue(square, allPieces);
            } else {
                eval -= LookupTable::lookupPSTValue(square, piece, allPieces);
            }
        }
    }

    return eval;
}

void BoardNode::checkPinsAndChecks(Colour colour, bool &check, bool &doubleCheck, U64 &kingLegalMoves, int &kingSquare, unordered_map<int, U64> &pins, U64 teamPieces, U64 opponentPieces, bool print) {
    U64 oppositionPawns;
    U64 oppositionKnights;
    U64 oppositionBishops;
    U64 oppositionRooks;
    U64 oppositionQueens;
    if (colour == Colour::WHITE) {
        kingSquare = getLSB(board->whiteKing);
        oppositionPawns = board->blackPawns;
        oppositionKnights = board->blackKnights;
        oppositionBishops = board->blackBishops;
        oppositionRooks = board->blackRooks;
        oppositionQueens = board->blackQueens;
    } else {
        kingSquare = getLSB(board->blackKing);
        oppositionPawns = board->whitePawns;
        oppositionKnights = board->whiteKnights;
        oppositionBishops = board->whiteBishops;
        oppositionRooks = board->whiteRooks;
        oppositionQueens = board->whiteQueens;
    }

    kingLegalMoves = LookupTable::lookupMusk(kingSquare, Piece::KING) & ~teamPieces;

    // look for pawn checks
    U64 checkMusk = LookupTable::lookupPawnControlMusk(kingSquare, colour);
    if ((checkMusk & oppositionPawns) != 0) {
        check = true;
        checkPathMusk = checkMusk & oppositionPawns;
    }

    // look for knight checks
    checkMusk = LookupTable::lookupMusk(kingSquare, Piece::KNIGHT);
    if (!check && (checkMusk & oppositionKnights) != 0) {
        check = true;
        checkPathMusk = checkMusk & oppositionKnights;
    }

    // quick way to ensure that check is possible
    U64 potentialCheckPaths = LookupTable::lookupMove(kingSquare, Piece::QUEEN, 0);
    if ((potentialCheckPaths & (oppositionBishops | oppositionRooks | oppositionQueens)) == 0) {
        return;
    }

    U64 checkDiagonalMusk = LookupTable::lookupMove(kingSquare, Piece::BISHOP, 0x0ULL);
    U64 diagonalOpponentAttackers = oppositionBishops | oppositionQueens;
    if ((checkDiagonalMusk & diagonalOpponentAttackers) != 0) {
        U64 attackers = checkDiagonalMusk & diagonalOpponentAttackers;
        while (attackers != 0) {
            int attacker = popLSB(attackers);
            int direction;
            U64 attackMusk;
            if (((attacker - kingSquare) % 7) == 0) {
                if (attacker - kingSquare > 0) {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, 7);
                    attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                    direction = 7;
                } else {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, -7);
                    attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                    direction = -7;
                }
            } else {
                if (attacker - kingSquare > 0) {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, 9);
                    attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                    direction = 9;
                } else {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, -9);
                    attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                    direction = -9;
                }
            }

            if (attackMusk & opponentPieces) {  // opponent piece in the way
                continue;
            } else if ((attackMusk & teamPieces) == 0) {  // check
                kingLegalMoves &= ~attackMusk;
                kingLegalMoves &= ~LookupTable::lookupRayMusk(kingSquare, -direction);
                if (check) {
                    doubleCheck = true;
                    return;
                } else {
                    check = true;
                    checkPathMusk = attackMusk | (0X1ULL << attacker);
                }
            } else if (__builtin_popcountll(attackMusk & teamPieces) == 1) {  // pin
                int pinnedPieceIndex = getLSB(attackMusk & teamPieces);
                U64 pinLegalMusk = attackMusk | (0x1ULL << attacker);
                pins[pinnedPieceIndex] = pinLegalMusk;
            }
        }
    }

    U64 checkStraightMusk = LookupTable::lookupMove(kingSquare, Piece::ROOK, 0x0ULL);
    U64 straightOpponentAttackers = oppositionRooks | oppositionQueens;

    if ((checkStraightMusk & straightOpponentAttackers) != 0) {
        U64 attackers = checkStraightMusk & straightOpponentAttackers;
        while (attackers != 0) {
            int attacker = popLSB(attackers);
            int direction;
            U64 attackMusk;
            if (((attacker - kingSquare) % 8) == 0) {
                if (attacker - kingSquare > 0) {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, 8);
                    attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                    direction = 8;
                } else {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, -8);
                    attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                    direction = -8;
                }
            } else {
                if (attacker - kingSquare > 0) {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, 1);
                    attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                    direction = 1;
                } else {
                    attackMusk = LookupTable::lookupRayMusk(kingSquare, -1);
                    attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                    direction = -1;
                }
            }

            if (attackMusk & opponentPieces) {  // piece in the way
                continue;
            } else if ((attackMusk & teamPieces) == 0) {  // check
                kingLegalMoves &= ~attackMusk;
                kingLegalMoves &= ~LookupTable::lookupRayMusk(kingSquare, -direction);
                if (check) {
                    doubleCheck = true;
                    return;
                } else {
                    check = true;
                    checkPathMusk = attackMusk | (0X1ULL << attacker);
                }
            } else if (__builtin_popcountll(attackMusk & teamPieces) == 1) {  // pin
                int pinnedPieceIndex = getLSB(attackMusk & teamPieces);
                U64 pinLegalMusk = attackMusk | (0x1ULL << attacker);
                pins[pinnedPieceIndex] = pinLegalMusk;
            }
        }
    }
}

void BoardNode::generateOpponentChecksAndUnsafeMusk(Colour myColour, U64 &unsafeMusk, U64 &diagonalChecks, U64 &straightChecks, U64 &knightChecks, U64 &pawnChecks, U64 teamPieces, U64 opponentPieces, unordered_map<int, U64> &improperEvasions, bool print) {
    // first need to consider pinned pieces
    U64 myBishops;
    U64 myRooks;
    U64 myQueens;
    int opponentKingSquare;
    vector<Piece> pieceGenerationOrder;
    Colour oppositionColour;
    if (myColour == Colour::WHITE) {
        opponentKingSquare = getLSB(board->blackKing);
        myBishops = board->whiteBishops;
        myRooks = board->whiteRooks;
        myQueens = board->whiteQueens;
        pieceGenerationOrder = {Piece::BLACKPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN, Piece::KING};
        oppositionColour = Colour::BLACK;
    } else {
        opponentKingSquare = getLSB(board->whiteKing);
        myBishops = board->blackBishops;
        myRooks = board->blackRooks;
        myQueens = board->blackQueens;
        pieceGenerationOrder = {Piece::WHITEPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN, Piece::KING};
        oppositionColour = Colour::WHITE;
    }
    U64 allPieces = teamPieces | opponentPieces;

    diagonalChecks = LookupTable::lookupMove(opponentKingSquare, Piece::BISHOP, allPieces);
    straightChecks = LookupTable::lookupMove(opponentKingSquare, Piece::ROOK, allPieces);
    knightChecks = LookupTable::lookupMusk(opponentKingSquare, Piece::KNIGHT);
    pawnChecks = LookupTable::lookupPawnControlMusk(opponentKingSquare, oppositionColour);

    unordered_map<int, U64> pins;
    // quick way to ensure that pin is possible
    U64 potentialCheckPaths = LookupTable::lookupMove(opponentKingSquare, Piece::QUEEN, 0);
    if ((potentialCheckPaths & (myBishops | myRooks | myQueens)) != 0) {
        U64 checkDiagonalMusk = LookupTable::lookupMove(opponentKingSquare, Piece::BISHOP, 0x0ULL);
        U64 diagonalOpponentAttackers = myBishops | myQueens;
        if ((checkDiagonalMusk & diagonalOpponentAttackers) != 0) {
            U64 attackers = checkDiagonalMusk & diagonalOpponentAttackers;
            while (attackers != 0) {
                int attacker = popLSB(attackers);
                int direction;
                U64 attackMusk;
                if (((attacker - opponentKingSquare) % 7) == 0) {
                    if (attacker - opponentKingSquare > 0) {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, 7);
                        attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                        direction = 7;
                    } else {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, -7);
                        attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                        direction = -7;
                    }
                } else {
                    if (attacker - opponentKingSquare > 0) {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, 9);
                        attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                        direction = 9;
                    } else {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, -9);
                        attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                        direction = -9;
                    }
                }

                if (attackMusk & teamPieces) {  // your piece is in the way
                    continue;
                } else if (__builtin_popcountll(attackMusk & opponentPieces) == 1) {  // pin
                    int pinnedPieceIndex = getLSB(attackMusk & opponentPieces);
                    U64 pinLegalMusk = attackMusk | (0x1ULL << attacker);
                    pins[pinnedPieceIndex] = pinLegalMusk;
                }
            }
        }

        U64 checkStraightMusk = LookupTable::lookupMove(opponentKingSquare, Piece::ROOK, 0x0ULL);
        U64 straightOpponentAttackers = myRooks | myQueens;
        if ((checkStraightMusk & straightOpponentAttackers) != 0) {
            U64 attackers = checkStraightMusk & straightOpponentAttackers;
            while (attackers != 0) {
                int attacker = popLSB(attackers);
                int direction;
                U64 attackMusk;
                if (((attacker - opponentKingSquare) % 8) == 0) {
                    if (attacker - opponentKingSquare > 0) {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, 8);
                        attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                        direction = 8;
                    } else {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, -8);
                        attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                        direction = -8;
                    }
                } else {
                    if (attacker - opponentKingSquare > 0) {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, 1);
                        attackMusk = clearBitsEqualGreaterThanIndex(attackMusk, attacker);
                        direction = 1;
                    } else {
                        attackMusk = LookupTable::lookupRayMusk(opponentKingSquare, -1);
                        attackMusk = clearBitsEqualLessThanIndex(attackMusk, attacker);
                        direction = -1;
                    }
                }

                if (attackMusk & teamPieces) {  // piece in the way
                    continue;
                } else if (__builtin_popcountll(attackMusk & opponentPieces) == 1) {  // pin
                    int pinnedPieceIndex = getLSB(attackMusk & opponentPieces);
                    U64 pinLegalMusk = attackMusk | (0x1ULL << attacker);
                    pins[pinnedPieceIndex] = pinLegalMusk;
                }
            }
        }
    }

    for (Piece piece : pieceGenerationOrder) {
        U64 existingPieces = board->getPieces(piece, oppositionColour);
        while (existingPieces != 0) {
            int initialSquare = popLSB(existingPieces);
            U64 controlMusk;
            switch (piece) {
                case (Piece::WHITEPAWN): {
                    controlMusk = LookupTable::lookupPawnControlMusk(initialSquare, oppositionColour);
                    break;
                }
                case (Piece::BLACKPAWN): {
                    controlMusk = LookupTable::lookupPawnControlMusk(initialSquare, oppositionColour);
                    break;
                }
                case (Piece::KNIGHT): {
                    controlMusk = LookupTable::lookupMusk(initialSquare, piece);
                    break;
                }
                case (Piece::KING): {
                    controlMusk = LookupTable::lookupMusk(initialSquare, piece);
                    break;
                }
                case (Piece::BISHOP): {
                    controlMusk = LookupTable::lookupMove(initialSquare, piece, allPieces);
                    if ((controlMusk & myBishops & myQueens) != 0) {
                        int directions[] = {NEGATIVE_DIAGONAL, POSITIVE_DIAGONAL, -POSITIVE_DIAGONAL, -POSITIVE_DIAGONAL};
                        for (int direction : directions) {
                            U64 checkRay = LookupTable::lookupRayMusk(initialSquare, direction);
                            U64 victims = checkRay & myBishops & myQueens;
                            if (__builtin_popcountll(victims) == 1) {
                                int victimKey = getLSB(victims);
                                improperEvasions[victimKey] = checkRay;
                            } else {
                                if (direction > 0) {
                                    improperEvasions[getLSB(victims)] = checkRay;
                                } else {
                                    improperEvasions[getMSB(victims)] = checkRay;
                                }
                            }
                        }
                    }
                    break;
                }
                case (Piece::ROOK): {
                    controlMusk = LookupTable::lookupMove(initialSquare, piece, allPieces);
                    if ((controlMusk & myRooks & myQueens) != 0) {
                        int directions[] = {VERTICAL, HORIZONTAL, -VERTICAL, -HORIZONTAL};
                        for (int direction : directions) {
                            U64 checkRay = LookupTable::lookupRayMusk(initialSquare, direction);
                            U64 victims = checkRay & myRooks & myQueens;
                            if (__builtin_popcountll(victims) == 1) {
                                int victimKey = getLSB(victims);
                                improperEvasions[victimKey] = checkRay;
                            } else {
                                if (direction > 0) {
                                    improperEvasions[getLSB(victims)] = checkRay;
                                } else {
                                    improperEvasions[getMSB(victims)] = checkRay;
                                }
                            }
                        }
                    }
                    break;
                }
                case (Piece::QUEEN): {
                    controlMusk = LookupTable::lookupMove(initialSquare, piece, allPieces);
                    break;
                }
            }

            if (pins.count(initialSquare) > 0) {
                controlMusk &= pins[initialSquare];
            }

            unsafeMusk |= controlMusk;
        }
    }
}

void BoardNode::generateMoves(Colour colour, bool print) {
    U64 teamPieces;
    U64 opponentPieces;
    U64 allPieces;
    vector<Piece> pieceGenerationOrder;
    int enPassantCaptureIndex = -1;
    if (colour == Colour::WHITE) {
        pieceGenerationOrder = {Piece::WHITEPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        teamPieces = board->getWhitePiecesMusk();
        opponentPieces = board->getBlackPiecesMusk();
        if (lastDoublePawnMoveIndex != 1) {
            enPassantCaptureIndex = lastDoublePawnMoveIndex + 8;
        }
    } else {
        pieceGenerationOrder = {Piece::BLACKPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN};
        teamPieces = board->getBlackPiecesMusk();
        opponentPieces = board->getWhitePiecesMusk();
        if (lastDoublePawnMoveIndex != 1) {
            enPassantCaptureIndex = lastDoublePawnMoveIndex - 8;
        }
    }
    allPieces = teamPieces | opponentPieces;

    bool check = false;
    bool doubleCheck = false;
    U64 kingLegalMoves;
    int kingSquare;
    unordered_map<int, U64> pins;
    checkPinsAndChecks(colour, check, doubleCheck, kingLegalMoves, kingSquare, pins, teamPieces, opponentPieces, false);

    U64 unsafeMusk = 0x0ULL;
    U64 diagonalChecks;
    U64 straightChecks;
    U64 knightChecks;
    U64 pawnChecks;
    unordered_map<int, U64> improperEvasions;
    generateOpponentChecksAndUnsafeMusk(colour, unsafeMusk, diagonalChecks, straightChecks, knightChecks, pawnChecks, teamPieces, opponentPieces, improperEvasions, false);

    if (print && check) {
        cout << "King is in check" << endl;
        bool check = false;
        bool doubleCheck = false;
        U64 kingLegalMoves;
        int kingSquare;
        unordered_map<int, U64> pins;
        checkPinsAndChecks(colour, check, doubleCheck, kingLegalMoves, kingSquare, pins, teamPieces, opponentPieces, true);

        U64 unsafeMusk = 0x0ULL;
        U64 diagonalChecks;
        U64 straightChecks;
        U64 knightChecks;
        U64 pawnChecks;
        unordered_map<int, U64> improperEvasions;
        generateOpponentChecksAndUnsafeMusk(colour, unsafeMusk, diagonalChecks, straightChecks, knightChecks, pawnChecks, teamPieces, opponentPieces, improperEvasions, false);
        cout << "unsafe musk" << endl;
        printBitboard(unsafeMusk, cout);
    }

    for (Piece piece : pieceGenerationOrder) {
        if (doubleCheck) {  // must move king only
            break;
        }
        U64 existingPieces = board->getPieces(piece, colour);
        while (existingPieces != 0) {
            int initialSquare = popLSB(existingPieces);
            U64 legalMoves;
            if ((piece == Piece::WHITEPAWN || piece == Piece::BLACKPAWN) && (lastDoublePawnMoveIndex != -1)) {
                legalMoves = (LookupTable::lookupMove(initialSquare, piece, allPieces | (0x1ULL << enPassantCaptureIndex))) & ~teamPieces;
            } else if (piece == Piece::KNIGHT) {
                legalMoves = LookupTable::lookupMusk(initialSquare, piece) & ~teamPieces;
            } else {
                legalMoves = LookupTable::lookupMove(initialSquare, piece, allPieces) & ~teamPieces;
            }

            if (pins.count(initialSquare) > 0) {
                legalMoves &= pins[initialSquare];
            }

            if (check) {
                // consider edge case where you can stop pawn check via en passant
                if ((lastDoublePawnMoveIndex != -1) && (piece == Piece::WHITEPAWN || piece == Piece::BLACKPAWN) &&
                    getBit(checkPathMusk, lastDoublePawnMoveIndex) &&
                    getBit(legalMoves, enPassantCaptureIndex)) {
                    legalMoves = 0x0ULL;
                    setBit(legalMoves, enPassantCaptureIndex);
                } else {
                    legalMoves &= checkPathMusk;
                }
            }

            while (legalMoves != 0) {
                int newSquare = popLSB(legalMoves);
                double moveVal = 0;
                int8_t flag = 0b0000;
                bool isMoveCheck = false;

                bool safe = true;
                if (getBit(unsafeMusk, newSquare)) {  // move to an unsafe square
                    moveVal *= 0.05;
                    moveVal -= board->getPieceValue(piece);
                    safe = false;
                } else if (getBit(unsafeMusk, initialSquare)) {  // evasions
                    if ((piece == Piece::BISHOP) || (piece == Piece::ROOK) || (piece == Piece::QUEEN)) {
                        if ((improperEvasions.count(initialSquare) > 0) && (!getBit(improperEvasions[initialSquare], newSquare))) {
                            moveVal += board->getPieceValue(piece);
                        }
                    } else {
                        moveVal += board->getPieceValue(piece);
                    }
                }

                switch (piece) {
                    case Piece::WHITEPAWN: {
                        if (newSquare >= 56) {
                            flag = Move::queenPromotion;
                        } else if (abs(newSquare - initialSquare) == 16) {
                            flag = Move::pawnDoubleMove;
                        } else if (newSquare == enPassantCaptureIndex) {
                            flag = Move::enPassant;
                            // also need to consider edge case where en passant is pinned from side (extremely rare but possible)
                            if (getBit(RANK_5, kingSquare)) {
                                if (kingSquare > lastDoublePawnMoveIndex) {  // king is on the right side
                                    U64 ray = LookupTable::lookupRayMusk(kingSquare, -HORIZONTAL);
                                    if (ray & ((board->blackRooks) | (board->blackQueens))) {
                                        int nearestThreat = getLSB((board->blackRooks) | (board->blackQueens));
                                        U64 inBetweenRay = clearBitsEqualLessThanIndex(ray, nearestThreat);
                                        inBetweenRay &= allPieces;
                                        if (__builtin_popcountll(inBetweenRay) == 2) {
                                            continue;
                                        }
                                    }
                                } else {  // king is on the left side
                                    U64 ray = LookupTable::lookupRayMusk(kingSquare, HORIZONTAL);
                                    if (ray & ((board->blackRooks) | (board->blackQueens))) {
                                        int nearestThreat = getLSB((board->blackRooks) | (board->blackQueens));
                                        U64 inBetweenRay = clearBitsEqualGreaterThanIndex(ray, nearestThreat);
                                        inBetweenRay &= allPieces;
                                        if (__builtin_popcountll(inBetweenRay) == 2) {
                                            continue;
                                        }
                                    }
                                }
                            }
                        } else {
                            flag = Move::pawnMove;
                        }

                        if (safe) {
                            if (newSquare >= 48) {
                                moveVal += 1;
                            } else {
                                moveVal += (LookupTable::lookupPawnPSTValue(newSquare, board->getPieces(Piece::BLACKPAWN, Colour::BLACK), colour)) - (LookupTable::lookupPawnPSTValue(initialSquare, board->getPieces(Piece::BLACKPAWN, Colour::BLACK), colour));
                            }
                        }

                        if (getBit(pawnChecks, newSquare)) {  // check
                            isMoveCheck = true;
                            if (safe) {
                                moveVal += 0.1;
                            } else {
                                moveVal += 2.6;
                            }
                        }
                        break;
                    }
                    case Piece::BLACKPAWN: {
                        if (newSquare < 8) {
                            flag = Move::queenPromotion;
                        } else if (abs(newSquare - initialSquare) == 16) {
                            flag = Move::pawnDoubleMove;
                        } else if (newSquare == enPassantCaptureIndex) {
                            flag = Move::enPassant;
                            // also need to consider edge case where en passant is pinned from side (extremely rare but possible)
                            if (getBit(RANK_4, kingSquare)) {
                                if (kingSquare > lastDoublePawnMoveIndex) {  // king is on the right side
                                    U64 ray = LookupTable::lookupRayMusk(kingSquare, -HORIZONTAL);
                                    if (ray & ((board->whiteRooks) | (board->whiteQueens))) {
                                        int nearestThreat = getLSB((board->whiteRooks) | (board->whiteQueens));
                                        U64 inBetweenRay = clearBitsEqualLessThanIndex(ray, nearestThreat);
                                        inBetweenRay &= allPieces;
                                        if (__builtin_popcountll(inBetweenRay) == 2) {
                                            continue;
                                        }
                                    }
                                } else {  // king is on the left side
                                    U64 ray = LookupTable::lookupRayMusk(kingSquare, HORIZONTAL);
                                    if (ray & ((board->whiteRooks) | (board->whiteQueens))) {
                                        int nearestThreat = getLSB((board->whiteRooks) | (board->whiteQueens));
                                        U64 inBetweenRay = clearBitsEqualGreaterThanIndex(ray, nearestThreat);
                                        inBetweenRay &= allPieces;
                                        if (__builtin_popcountll(inBetweenRay) == 2) {
                                            continue;
                                        }
                                    }
                                }
                            }
                        } else {
                            flag = Move::pawnMove;
                        }

                        if (safe) {
                            if (newSquare <= 15) {
                                moveVal += 1;
                            } else {
                                moveVal += (LookupTable::lookupPawnPSTValue(newSquare, board->getPieces(Piece::WHITEPAWN, Colour::WHITE), colour)) - (LookupTable::lookupPawnPSTValue(initialSquare, board->getPieces(Piece::WHITEPAWN, Colour::WHITE), colour));
                            }
                        }

                        if (getBit(pawnChecks, newSquare)) {  // check
                            isMoveCheck = true;
                            if (safe) {
                                moveVal += 0.1;
                            } else {
                                moveVal += 2.6;
                            }
                        }
                        break;
                    }
                    case Piece::KNIGHT: {
                        flag = Move::knightMove;
                        if (safe) {
                            U64 newAllPieces = allPieces;
                            clearBit(newAllPieces, initialSquare);
                            moveVal += (LookupTable::lookupKnightPSTValue(newSquare, newAllPieces)) - (LookupTable::lookupKnightPSTValue(initialSquare, allPieces));
                        }

                        if (getBit(knightChecks, newSquare)) {  // check
                            isMoveCheck = true;
                            if (safe) {
                                moveVal += 0.1;
                            } else {
                                moveVal += 2.6;
                            }
                        }
                        break;
                    }
                    case Piece::BISHOP: {
                        flag = Move::bishopMove;
                        if (safe) {
                            U64 newAllPieces = allPieces;
                            clearBit(newAllPieces, initialSquare);
                            moveVal += (LookupTable::lookupPSTValue(newSquare, Piece::BISHOP, newAllPieces)) - (LookupTable::lookupPSTValue(initialSquare, Piece::BISHOP, allPieces));
                        }

                        if (getBit(diagonalChecks, newSquare)) {  // check
                            isMoveCheck = true;
                            if (safe) {
                                moveVal += 0.1;
                            } else {
                                moveVal += 2.6;
                            }
                        }
                        break;
                    }
                    case Piece::ROOK: {
                        flag = Move::rookMove;
                        if (safe) {
                            U64 newAllPieces = allPieces;
                            clearBit(newAllPieces, initialSquare);
                            moveVal += (LookupTable::lookupPSTValue(newSquare, Piece::ROOK, newAllPieces)) - (LookupTable::lookupPSTValue(initialSquare, Piece::ROOK, allPieces));
                        }

                        if (getBit(straightChecks, newSquare)) {  // check
                            isMoveCheck = true;
                            if (safe) {
                                moveVal += 0.1;
                            } else {
                                moveVal += 2.6;
                            }
                        }
                        break;
                    }
                    case Piece::QUEEN: {
                        flag = Move::queenMove;
                        if (safe) {
                            U64 newAllPieces = allPieces;
                            clearBit(newAllPieces, initialSquare);
                            moveVal += (LookupTable::lookupPSTValue(newSquare, Piece::QUEEN, newAllPieces)) - (LookupTable::lookupPSTValue(initialSquare, Piece::QUEEN, allPieces));
                        }

                        if (getBit(diagonalChecks, newSquare) || getBit(straightChecks, newSquare)) {  // check
                            isMoveCheck = true;
                            if (safe) {
                                moveVal += 0.1;
                            } else {
                                moveVal += 2.6;
                            }
                        }
                        break;
                    }
                    default:
                        throw logic_error("Invalid piece");
                        break;
                }

                int8_t captureFlag = 0b0000;
                if (getBit(opponentPieces, newSquare)) {  // if capture
                    try {
                        moveVal += board->findPiece(newSquare, !colour, captureFlag);
                    } catch (exception &e) {
                        cout << "Attempted to capture from " << initialSquare << " to " << newSquare << endl;
                        printBoardOnly(cout);
                        BoardNode *getParent = parent;
                        Colour switchColour = colour;
                        while (getParent) {
                            getParent->printBoardOnly(cout);
                            if (switchColour == Colour::WHITE) {
                                switchColour = Colour::BLACK;
                            } else {
                                switchColour = Colour::WHITE;
                            }
                            bool check = false;
                            bool doubleCheck = false;
                            U64 kingLegalMoves;
                            int kingSquare;
                            unordered_map<int, U64> pins;
                            getParent->checkPinsAndChecks(switchColour, check, doubleCheck, kingLegalMoves, kingSquare, pins, teamPieces, opponentPieces, true);
                            U64 unsafeMusk = 0x0ULL;
                            U64 diagonalChecks;
                            U64 straightChecks;
                            U64 knightChecks;
                            U64 pawnChecks;
                            unordered_map<int, U64> improperEvasions;
                            getParent->generateOpponentChecksAndUnsafeMusk(switchColour, unsafeMusk, diagonalChecks, straightChecks, knightChecks, pawnChecks, teamPieces, opponentPieces, improperEvasions, true);
                            getParent = getParent->parent;
                        }
                        throw;
                    }
                } else if (flag == Move::enPassant) {
                    moveVal += 1;
                }

                unique_ptr<Move> newMove = make_unique<Move>(initialSquare, newSquare, flag, captureFlag, moveVal, isMoveCheck);
                moves.emplace_back(move(newMove));

                if (flag == Move::queenPromotion) {  // very unlikely to consider down promoting instead of just promoting to queen (so we should consider last)
                    unique_ptr<Move> newMove1 = make_unique<Move>(initialSquare, newSquare, 0b0101, captureFlag, moveVal - 100, isMoveCheck);
                    moves.emplace_back(move(newMove1));
                    unique_ptr<Move> newMove2 = make_unique<Move>(initialSquare, newSquare, 0b0110, captureFlag, moveVal - 101, isMoveCheck);
                    moves.emplace_back(move(newMove2));
                    unique_ptr<Move> newMove3 = make_unique<Move>(initialSquare, newSquare, 0b0111, captureFlag, moveVal - 102, isMoveCheck);
                    moves.emplace_back(move(newMove3));
                }
            }
        }
    }

    // generate king moves below
    while (kingLegalMoves != 0) {
        int newSquare = popLSB(kingLegalMoves);

        if (getBit(unsafeMusk, newSquare)) {
            continue;
        }

        double moveVal = 0;
        int8_t flag = Move::kingMove;
        int8_t captureFlag = 0b0000;

        if (getBit(opponentPieces, newSquare)) {  // if capture
            try {
                moveVal += board->findPiece(newSquare, !colour, captureFlag);
            } catch (exception &e) {
                cout << "Attempted to capture from " << kingSquare << " to " << newSquare << endl;
                printBoardOnly(cout);
                BoardNode *getParent = parent;
                Colour switchColour = colour;
                while (getParent) {
                    getParent->printBoardOnly(cout);
                    if (switchColour == Colour::WHITE) {
                        switchColour = Colour::BLACK;
                    } else {
                        switchColour = Colour::WHITE;
                    }
                    bool check = false;
                    bool doubleCheck = false;
                    U64 kingLegalMoves;
                    int kingSquare;
                    unordered_map<int, U64> pins;
                    getParent->checkPinsAndChecks(switchColour, check, doubleCheck, kingLegalMoves, kingSquare, pins, teamPieces, opponentPieces, true);
                    U64 unsafeMusk = 0x0ULL;
                    U64 diagonalChecks;
                    U64 straightChecks;
                    U64 knightChecks;
                    U64 pawnChecks;
                    unordered_map<int, U64> improperEvasions;
                    getParent->generateOpponentChecksAndUnsafeMusk(switchColour, unsafeMusk, diagonalChecks, straightChecks, knightChecks, pawnChecks, teamPieces, opponentPieces, improperEvasions, true);
                    getParent = getParent->parent;
                }
                throw;
            }
        }

        unique_ptr<Move> newMove = make_unique<Move>(kingSquare, newSquare, flag, captureFlag, moveVal, false);
        moves.emplace_back(move(newMove));
    }

    // check castling
    if (colour == Colour::WHITE) {
        if (castleStatus.canWhiteKingCastleLeft() && ((whiteLeftCastle & allPieces) == 0)) {
            if (!getBit(unsafeMusk, 4) && !getBit(unsafeMusk, 1) && !getBit(unsafeMusk, 2) && !getBit(unsafeMusk, 3)) {
                if (getBit(straightChecks, 3)) {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 2, 0b0001, 0b0000, 2.6, true);
                    moves.emplace_back(move(newMove));
                } else {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 2, 0b0001, 0b0000, 2.6, false);
                    moves.emplace_back(move(newMove));
                }
            }
        }

        if (castleStatus.canWhiteKingCastleRight() && ((whiteRightCastle & allPieces) == 0)) {
            if (!getBit(unsafeMusk, 4) && !getBit(unsafeMusk, 5) && !getBit(unsafeMusk, 6)) {
                if (getBit(straightChecks, 5)) {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 6, 0b0001, 0b0000, 1.7, true);
                    moves.emplace_back(move(newMove));
                } else {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 6, 0b0001, 0b0000, 1.7, false);
                    moves.emplace_back(move(newMove));
                }
            }
        }
    } else {
        if (castleStatus.canBlackKingCastleLeft() &&
            ((blackLeftCastle & allPieces) == 0)) {
            if (!getBit(unsafeMusk, 60) && !getBit(unsafeMusk, 57) && !getBit(unsafeMusk, 58) && !getBit(unsafeMusk, 59)) {
                if (getBit(straightChecks, 59)) {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 58, 0b0001, 0b0000, 2.6, true);
                    moves.emplace_back(move(newMove));
                } else {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 58, 0b0001, 0b0000, 2.6, false);
                    moves.emplace_back(move(newMove));
                }
            }
        }

        if (castleStatus.canBlackKingCastleRight() &&
            ((blackRightCastle & allPieces) == 0)) {
            if (!getBit(unsafeMusk, 60) && !getBit(unsafeMusk, 61) && !getBit(unsafeMusk, 62)) {
                if (getBit(straightChecks, 61)) {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 62, 0b0001, 0b0000, 1.7, true);
                    moves.emplace_back(move(newMove));
                } else {
                    unique_ptr<Move> newMove = make_unique<Move>(kingSquare, 62, 0b0001, 0b0000, 1.7, false);
                    moves.emplace_back(move(newMove));
                }
            }
        }
    }

    sort(moves.begin(), moves.end(), [](const unique_ptr<Move> &lhs, const unique_ptr<Move> &rhs) {
        return lhs->value > rhs->value;
    });
}

void BoardNode::addPredictedBestMove(Colour colour) {
    if (moves.size() <= 0) {
        throw logic_error("Attempted to branch but there are no moves to branch");
    }

    unique_ptr<Board> newPosition = make_unique<Board>(*board);
    unique_ptr<Move> bestPredictedMove = move(moves.front());
    moves.erase(moves.begin());
    int flag = bestPredictedMove->getFlag();

    CastleStatus newCastleStatus = castleStatus;
    int newLastDoublePawnMoveIndex = -1;

    if (colour == Colour::WHITE) {
        switch (flag) {
            case Move::noFlag:
                throw logic_error("No flag");
                break;
            case Move::castle:
                newCastleStatus.disenableWhiteKingCastleLeft();
                newCastleStatus.disenableWhiteKingCastleRight();
                newPosition->whiteKing ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                if (bestPredictedMove->getToSquare() == 2) {
                    newPosition->whiteRooks ^= (0x1ULL << 0) | (0x1ULL << 3);
                } else if (bestPredictedMove->getToSquare() == 6) {
                    newPosition->whiteRooks ^= (0x1ULL << 5) | (0x1ULL << 7);
                }
                children.emplace_back(make_unique<BoardNode>(move(newPosition), newLastDoublePawnMoveIndex, newCastleStatus, this));
                return;
            case Move::enPassant:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                clearBit(newPosition->blackPawns, lastDoublePawnMoveIndex);
                children.emplace_back(make_unique<BoardNode>(move(newPosition), newLastDoublePawnMoveIndex, newCastleStatus, this));
                return;
            case Move::pawnDoubleMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                newLastDoublePawnMoveIndex = bestPredictedMove->getToSquare();
                children.emplace_back(make_unique<BoardNode>(move(newPosition), newLastDoublePawnMoveIndex, newCastleStatus, this));
                return;
            case Move::queenPromotion:
                clearBit(newPosition->whitePawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->whiteQueens, bestPredictedMove->getToSquare());
                break;
            case Move::rookPromotion:
                clearBit(newPosition->whitePawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->whiteRooks, bestPredictedMove->getToSquare());
                break;
            case Move::knightPromotion:
                clearBit(newPosition->whitePawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->whiteKnights, bestPredictedMove->getToSquare());
                break;
            case Move::bishopPromotion:
                clearBit(newPosition->whitePawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->whiteBishops, bestPredictedMove->getToSquare());
                break;
            case Move::pawnMove:
                newPosition->whitePawns ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::knightMove:
                newPosition->whiteKnights ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::bishopMove:
                newPosition->whiteBishops ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::rookMove:
                newPosition->whiteRooks ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                if (bestPredictedMove->getFromSquare() == 0) {
                    newCastleStatus.disenableWhiteKingCastleLeft();
                } else if (bestPredictedMove->getFromSquare() == 7) {
                    newCastleStatus.disenableWhiteKingCastleRight();
                }
                break;
            case Move::queenMove:
                newPosition->whiteQueens ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::kingMove:
                newPosition->whiteKing ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                newCastleStatus.disenableWhiteKingCastleLeft();
                newCastleStatus.disenableWhiteKingCastleRight();
                break;
            default:
                throw logic_error("Undefined flag");
                break;
        }
    } else {
        switch (flag) {
            case Move::noFlag:
                throw logic_error("No flag");
                break;
            case Move::castle:
                newCastleStatus.disenableBlackKingCastleLeft();
                newCastleStatus.disenableBlackKingCastleRight();
                newPosition->blackKing ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                if (bestPredictedMove->getToSquare() == 58) {
                    newPosition->blackRooks ^= (0x1ULL << 56) | (0x1ULL << 59);
                } else if (bestPredictedMove->getToSquare() == 62) {
                    newPosition->blackRooks ^= (0x1ULL << 61) | (0x1ULL << 63);
                }
                children.emplace_back(make_unique<BoardNode>(move(newPosition), newLastDoublePawnMoveIndex, newCastleStatus, this));
                return;
            case Move::enPassant:
                newPosition->blackPawns ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                clearBit(newPosition->whitePawns, lastDoublePawnMoveIndex);
                children.emplace_back(make_unique<BoardNode>(move(newPosition), newLastDoublePawnMoveIndex, newCastleStatus, this));
                return;
            case Move::pawnDoubleMove:
                newPosition->blackPawns ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                newLastDoublePawnMoveIndex = bestPredictedMove->getToSquare();
                children.emplace_back(make_unique<BoardNode>(move(newPosition), newLastDoublePawnMoveIndex, newCastleStatus, this));
                return;
            case Move::queenPromotion:
                clearBit(newPosition->blackPawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->blackQueens, bestPredictedMove->getToSquare());
                break;
            case Move::rookPromotion:
                clearBit(newPosition->blackPawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->blackRooks, bestPredictedMove->getToSquare());
                break;
            case Move::knightPromotion:
                clearBit(newPosition->blackPawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->blackKnights, bestPredictedMove->getToSquare());
                break;
            case Move::bishopPromotion:
                clearBit(newPosition->blackPawns, bestPredictedMove->getFromSquare());
                setBit(newPosition->blackBishops, bestPredictedMove->getToSquare());
                break;
            case Move::pawnMove:
                newPosition->blackPawns ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::knightMove:
                newPosition->blackKnights ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::bishopMove:
                newPosition->blackBishops ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::rookMove:
                newPosition->blackRooks ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                if (bestPredictedMove->getFromSquare() == 56) {
                    newCastleStatus.disenableBlackKingCastleLeft();
                } else if (bestPredictedMove->getFromSquare() == 63) {
                    newCastleStatus.disenableBlackKingCastleRight();
                }
                break;
            case Move::queenMove:
                newPosition->blackQueens ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                break;
            case Move::kingMove:
                newPosition->blackKing ^= (0x1ULL << bestPredictedMove->getFromSquare()) | (0x1ULL << bestPredictedMove->getToSquare());
                newCastleStatus.disenableBlackKingCastleLeft();
                newCastleStatus.disenableBlackKingCastleRight();
                break;
            default:
                throw logic_error("Undefined flag");
                break;
        }
    }

    // REMINDER: You should return instead of break for special move cases

    if (bestPredictedMove->capture != 0b0000) {
        if (colour == Colour::WHITE) {
            switch (bestPredictedMove->capture) {
                case Move::pawnCapture:
                    newPosition->blackPawns ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                case Move::knightCapture:
                    newPosition->blackKnights ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                case Move::bishopCapture:
                    newPosition->blackBishops ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                case Move::rookCapture:
                    newPosition->blackRooks ^= (0x1ULL << bestPredictedMove->getToSquare());
                    if (bestPredictedMove->getToSquare() == 56) {
                        newCastleStatus.disenableBlackKingCastleLeft();
                    } else if (bestPredictedMove->getToSquare() == 63) {
                        newCastleStatus.disenableBlackKingCastleRight();
                    }
                    break;
                case Move::queenCapture:
                    newPosition->blackQueens ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                default:
                    throw logic_error("Undefined capture flag");
                    break;
            }
        } else {
            switch (bestPredictedMove->capture) {
                case Move::pawnCapture:
                    newPosition->whitePawns ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                case Move::knightCapture:
                    newPosition->whiteKnights ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                case Move::bishopCapture:
                    newPosition->whiteBishops ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                case Move::rookCapture:
                    newPosition->whiteRooks ^= (0x1ULL << bestPredictedMove->getToSquare());
                    if (bestPredictedMove->getToSquare() == 0) {
                        newCastleStatus.disenableWhiteKingCastleLeft();
                    } else if (bestPredictedMove->getToSquare() == 7) {
                        newCastleStatus.disenableWhiteKingCastleRight();
                    }
                    break;
                case Move::queenCapture:
                    newPosition->whiteQueens ^= (0x1ULL << bestPredictedMove->getToSquare());
                    break;
                default:
                    throw logic_error("Undefined capture flag");
                    break;
            }
        }
    }

    children.emplace_back(make_unique<BoardNode>(move(newPosition), newLastDoublePawnMoveIndex, newCastleStatus, this));
}

bool BoardNode::isWorthChecking(double limit) {
    auto& firstMove = moves.at(0);
    if (firstMove->value >= limit) {
        return true;
    }
    return false;
}

ostream &BoardNode::printBoardOnly(ostream &out) {
    out << *(board) << endl;
    return out;
}

ostream &BoardNode::printChildrenValues(ostream &out) {
    for (size_t i = 0; i < children.size(); i++) {
        cout << children[i]->getValue() << endl;
    }
    return out;
}

string indexToChessSquare(int index) {
    char file = 'a' + (index % 8);
    int rank = 1 + (index / 8);
    return string(1, file) + to_string(rank);
}

ostream &BoardNode::printChildrenMoveNotation(ostream &out) {
    for (auto &move : moves) {
        cout << "(move value " << move->value << ") "
             << indexToChessSquare(move->getFromSquare()) << " "
             << indexToChessSquare(move->getToSquare()) << endl;
    }
    return out;
}

string generateRandomID() {
    std::string charset =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> indexDist(0, charset.size() - 1);
    std::string id;
    for (int i = 0; i < 8; ++i) {
        id += charset[indexDist(gen)];
    }
    return id;
}

ostream &BoardNode::printChildrenTree(ostream &out) {
    string randomID = generateRandomID();
    cout << randomID << endl;
    printBoardOnly(cout);
    cout << "children of " << randomID << endl;
    for (auto &child : children) {
        cout << "Value of following board ID (from parent id " << randomID
             << "): " << child->getValue() << endl;
        child->printBoardOnly(cout);
        if (child->getChildren().size() == 0) {
            cout << "This board does not have any children" << endl;
            continue;
        } else {
            cout << "Branch into child displayed below" << endl;
            child->printChildrenTree(out);
        }
    }
    return out;
}

ostream &BoardNode::printChildrenValue(ostream &out) {
    for (auto &child : children) {
        cout << child->getValue() << endl;
    }
    return out;
}

vector<unique_ptr<BoardNode>> &BoardNode::getChildren() { return children; }

BoardNode::~BoardNode() {}

void BoardNode::clearMoves() { moves.clear(); }

bool BoardNode::containsMove(int fromSquare, int toSquare) {
    for (auto &move : moves) {
        if (move->getFromSquare() == fromSquare &&
            move->getToSquare() == toSquare) {
            return true;
        }
    }
    return false;
}

void branchToChild(unique_ptr<BoardNode> &boardNode, size_t childIndex) {
    boardNode = move(boardNode->children[childIndex]);
}

size_t BoardNode::branchToChildInput(int fromSquare, int toSquare, Colour colour) {
    vector<Piece> pieceCheckOrder;

    if (colour == Colour::WHITE) {
        pieceCheckOrder = {Piece::WHITEPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::KING, Piece::ROOK, Piece::QUEEN};
    } else {
        pieceCheckOrder = {Piece::BLACKPAWN, Piece::KNIGHT, Piece::BISHOP, Piece::KING, Piece::ROOK, Piece::QUEEN};
    }

    size_t childIndex = -1;
    for (int i = 0; i < children.size(); i++) {
        unique_ptr<BoardNode> &child = children[i];
        for (Piece piece : pieceCheckOrder) {
            U64 checkNewPos = child->board->getPieces(piece, colour);

            if ((colour == Colour::WHITE) && (piece == Piece::WHITEPAWN) && (toSquare >= 56)) {
                U64 checkQueen = child->board->getPieces(Piece::QUEEN, colour);
                if (getBit(checkQueen, toSquare) && !getBit(checkQueen, fromSquare)) {
                    U64 checkPieces = board->getPieces(piece, colour);
                    if (!getBit(checkPieces, toSquare) && getBit(checkPieces, fromSquare)) {
                        childIndex = i;
                    }
                }
            } else if ((colour == Colour::BLACK) && (piece == Piece::BLACKPAWN) && (toSquare <= 7)) {
                U64 checkQueen = child->board->getPieces(Piece::QUEEN, colour);
                if (getBit(checkQueen, toSquare) && !getBit(checkQueen, fromSquare)) {
                    U64 checkPieces = board->getPieces(piece, colour);
                    if (!getBit(checkPieces, toSquare) && getBit(checkPieces, fromSquare)) {
                        childIndex = i;
                    }
                }
            }

            if (getBit(checkNewPos, toSquare) && !getBit(checkNewPos, fromSquare)) {
                U64 checkPieces = board->getPieces(piece, colour);
                if (!getBit(checkPieces, toSquare) && getBit(checkPieces, fromSquare)) {
                    childIndex = i;
                }
            }
        }
    }

    return childIndex;
}

void BoardNode::setValue(double value) { this->value = value; }

double BoardNode::getValue() const { return value; }

vector<unique_ptr<Move>> &BoardNode::getMoves() { return moves; }
