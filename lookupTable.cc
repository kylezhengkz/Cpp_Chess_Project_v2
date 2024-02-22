#include "lookupTable.h"
U64 LookupTable::whitePawnMusks[64];
U64 LookupTable::blackPawnMusks[64];
U64 LookupTable::whitePawnControlMusks[64];
U64 LookupTable::blackPawnControlMusks[64];
U64 LookupTable::bishopMusks[64];
U64 LookupTable::rookMusks[64];
U64 LookupTable::queenMusks[64];
U64 LookupTable::knightMusks[64];
U64 LookupTable::kingMusks[64];
U64 LookupTable::rayMusks[8][64] = {};
int LookupTable::whitePawnBlockerShifts[64];
int LookupTable::blackPawnBlockerShifts[64];
int LookupTable::bishopBlockerShifts[64];
int LookupTable::rookBlockerShifts[64];
U64 LookupTable::whitePawnMagicArray[64];
U64 LookupTable::blackPawnMagicArray[64];
U64 LookupTable::bishopMagicArray[64];
U64 LookupTable::rookMagicArray[64];
unordered_map<pair<int, U64>, U64, LookupTable::PairHash, LookupTable::PairEqual> LookupTable::whitePawnMoves;
unordered_map<pair<int, U64>, U64, LookupTable::PairHash, LookupTable::PairEqual> LookupTable::blackPawnMoves;
unordered_map<pair<int, U64>, U64, LookupTable::PairHash, LookupTable::PairEqual> LookupTable::bishopMoves;
unordered_map<pair<int, U64>, U64, LookupTable::PairHash, LookupTable::PairEqual> LookupTable::rookMoves;

U64 LookupTable::lookupPawnControlMusk(int key, Colour colour) {
    if (colour == Colour::WHITE) {
        return whitePawnControlMusks[key];
    } else {
        return blackPawnControlMusks[key];
    }
}

U64 LookupTable::lookupMusk(int key, Piece piece) {
    switch (piece) {
        case (Piece::KNIGHT):
            return knightMusks[key];
            break;
        case (Piece::KING):
            return kingMusks[key];
            break;
        case (Piece::QUEEN):
            return queenMusks[key];
            break;
        case (Piece::BISHOP):
            return bishopMusks[key];
            break;
        case (Piece::ROOK):
            return rookMusks[key];
            break;
        default:
            throw logic_error("Attempted to search an invalid musk");
            break;
    }
}

// REPLACE .at(key) with [key] when done and remove try catches (TEMP)
U64 LookupTable::lookupMove(int square, Piece piece, U64 allPieces) {
    switch (piece) {
        case (Piece::WHITEPAWN): {
            U64 blockers = whitePawnMusks[square] & allPieces;
            pair<int, U64> key = make_pair(square, blockers);
            try {
                return whitePawnMoves.at(key);
            } catch (exception& e) {
                cout << key.first << endl;
                printBitboard(key.second, cout);
                throw e;
            }
            break;
        } case (Piece::BLACKPAWN): {
            U64 blockers = blackPawnMusks[square] & allPieces;
            pair<int, U64> key = make_pair(square, blockers);
            try {
                return blackPawnMoves.at(key);
            } catch (exception& e) {
                cout << key.first << endl;
                printBitboard(key.second, cout);
                throw e;
            }
            break;
        } case (Piece::BISHOP): {
            U64 blockers = bishopMusks[square] & allPieces;
            pair<int, U64> key = make_pair(square, blockers);
            try {
                return bishopMoves.at(key);
            } catch (exception& e) {
                cout << key.first << endl;
                printBitboard(key.second, cout);
                throw e;
            }
            break;
        } case (Piece::ROOK): {
            U64 blockers = rookMusks[square] & allPieces;
            pair<int, U64> key = make_pair(square, blockers);
            try {
                return rookMoves.at(key);
            } catch (exception& e) {
                cout << key.first << endl;
                printBitboard(key.second, cout);
                throw e;
            }
            break;
        } case (Piece::QUEEN): {
            U64 verticalBlockers = rookMusks[square] & allPieces;
            U64 diagonalBlockers = bishopMusks[square] & allPieces;
            pair<int, U64> verticalKey = make_pair(square, verticalBlockers);
            pair<int, U64> diagonalKey = make_pair(square, diagonalBlockers);
            try {
                return rookMoves.at(verticalKey) | bishopMoves.at(diagonalKey);
            } catch (exception& e) {
                cout << verticalKey.first << endl;
                printBitboard(verticalKey.second, cout);
                cout << diagonalKey.first << endl;
                printBitboard(diagonalKey.second, cout);
                throw e;
            }
            break;
        } default:
            throw logic_error("Attempted to search an invalid musk");
            break;
    }
}

U64 LookupTable::lookupRayMusk(int square, int direction) {
    switch (direction) {
        case(VERTICAL): {
            return rayMusks[UPINDEX][square];
            break;
        } case(-VERTICAL): {
            return rayMusks[DOWNINDEX][square];
            break;
        } case(HORIZONTAL): {
            return rayMusks[RIGHTINDEX][square];
            break;
        } case(-HORIZONTAL): {
            return rayMusks[LEFTINDEX][square];
            break;
        } case(POSITIVE_DIAGONAL): {
            return rayMusks[UPRIGHTINDEX][square];
            break;
        } case(-POSITIVE_DIAGONAL): {
            return rayMusks[DOWNLEFTINDEX][square];
            break;
        } case(NEGATIVE_DIAGONAL): {
            return rayMusks[UPLEFTINDEX][square];
            break;
        } case(-NEGATIVE_DIAGONAL): {
            return rayMusks[DOWNRIGHTINDEX][square];
            break;
        } default:
            throw logic_error("Attempted to lookup an invalid ray musk");
            break;
    }
}

void LookupTable::setRayMusks() {
    U64 fromSquare = 0x1ULL;
    for (int i = 0; i < 64; i++) {
        U64 fromSlidingSquare = fromSquare >> POSITIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~H_FILE & fromSlidingSquare) != 0) {
            rayMusks[DOWNLEFTINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> POSITIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare >> NEGATIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~A_FILE & fromSlidingSquare) != 0) {
            rayMusks[DOWNRIGHTINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> NEGATIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare << POSITIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~A_FILE & fromSlidingSquare) != 0) {
            rayMusks[UPRIGHTINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << POSITIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare << NEGATIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~H_FILE & fromSlidingSquare) != 0) {
            rayMusks[UPLEFTINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << NEGATIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare >> VERTICAL;
        while (fromSlidingSquare != 0 && (~RANK_8 & fromSlidingSquare) != 0) {
            rayMusks[DOWNINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> VERTICAL;
        }

        fromSlidingSquare = fromSquare << VERTICAL;
        while (fromSlidingSquare != 0 && (~RANK_1 & fromSlidingSquare) != 0) {
            rayMusks[UPINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << VERTICAL;
        }

        fromSlidingSquare = fromSquare >> HORIZONTAL;
        while (fromSlidingSquare != 0 && (~H_FILE & fromSlidingSquare) != 0) {
            rayMusks[LEFTINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> HORIZONTAL;
        }

        fromSlidingSquare = fromSquare << HORIZONTAL;
        while (fromSlidingSquare != 0 && (~A_FILE & fromSlidingSquare) != 0) {
            rayMusks[RIGHTINDEX][i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << HORIZONTAL;
        }
        fromSquare = fromSquare << 1;
    }
}

void LookupTable::setMusks() {
    U64 fromSquare = 0x1ULL;
    for (int i = 0; i < 64; i++) {
        kingMusks[i] = (((fromSquare << NEGATIVE_DIAGONAL) | (fromSquare >> HORIZONTAL) | (fromSquare >> POSITIVE_DIAGONAL)) & ~H_FILE)
        | (((fromSquare >> NEGATIVE_DIAGONAL) | (fromSquare << HORIZONTAL) | (fromSquare << POSITIVE_DIAGONAL)) & ~A_FILE)
        | (fromSquare << VERTICAL | fromSquare >> VERTICAL);
            
        knightMusks[i] = (((fromSquare >> 6)  | (fromSquare << 10)) & ~A_FILE & ~B_FILE)
        | (((fromSquare >> 10) | (fromSquare << 6)) & ~G_FILE & ~H_FILE)
        | (((fromSquare >> 15) | (fromSquare << 17)) & ~A_FILE)
        | (((fromSquare << 15) | (fromSquare >> 17)) & ~H_FILE);
        
        if (i <= 55) {
            whitePawnMusks[i] = (((fromSquare << NEGATIVE_DIAGONAL)) & ~H_FILE)
            | (((fromSquare << POSITIVE_DIAGONAL)) & ~A_FILE)
            | (fromSquare << VERTICAL);
            if ((i / 8) == 1) {
                whitePawnMusks[i] |= (fromSquare << 2*VERTICAL);
            }

            whitePawnControlMusks[i] = (((fromSquare << NEGATIVE_DIAGONAL)) & ~H_FILE)
            | (((fromSquare << POSITIVE_DIAGONAL)) & ~A_FILE);
        }

        if (i >= 8) {
            blackPawnMusks[i] = (((fromSquare >> NEGATIVE_DIAGONAL)) & ~A_FILE)
            | (((fromSquare >> POSITIVE_DIAGONAL)) & ~H_FILE)
            | (fromSquare >> VERTICAL);
            if ((i / 8) == 6) {
                blackPawnMusks[i] |= (fromSquare >> 2*VERTICAL);
            }

            blackPawnControlMusks[i] = (((fromSquare >> NEGATIVE_DIAGONAL)) & ~A_FILE)
            | (((fromSquare >> POSITIVE_DIAGONAL)) & ~H_FILE);
        }
        
        U64 fromSlidingSquare = fromSquare >> POSITIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~A_FILE & ~H_FILE & ~RANK_1 & fromSlidingSquare) != 0) {
            bishopMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> POSITIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare >> NEGATIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~A_FILE & ~H_FILE & ~RANK_1 & fromSlidingSquare) != 0) {
            bishopMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> NEGATIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare << POSITIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~A_FILE & ~H_FILE & ~RANK_8 & fromSlidingSquare) != 0) {
            bishopMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << POSITIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare << NEGATIVE_DIAGONAL;
        while (fromSlidingSquare != 0 && (~A_FILE & ~H_FILE & ~RANK_8 & fromSlidingSquare) != 0) {
            bishopMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << NEGATIVE_DIAGONAL;
        }

        fromSlidingSquare = fromSquare >> VERTICAL;
        while (fromSlidingSquare != 0 && (~RANK_1 & ~RANK_8 & fromSlidingSquare) != 0) {
            rookMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> VERTICAL;
        }

        fromSlidingSquare = fromSquare << VERTICAL;
        while (fromSlidingSquare != 0 && (~RANK_1 & ~RANK_8 & fromSlidingSquare) != 0) {
            rookMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << VERTICAL;
        }

        fromSlidingSquare = fromSquare >> HORIZONTAL;
        while (fromSlidingSquare != 0 && (~A_FILE & ~H_FILE & fromSlidingSquare) != 0) {
            rookMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare >> HORIZONTAL;
        }

        fromSlidingSquare = fromSquare << HORIZONTAL;
        while (fromSlidingSquare != 0 && (~A_FILE & ~H_FILE & fromSlidingSquare) != 0) {
            rookMusks[i] |= fromSlidingSquare;
            queenMusks[i] |= fromSlidingSquare;
            fromSlidingSquare = fromSlidingSquare << HORIZONTAL;
        }
        fromSquare = fromSquare << 1;
    }
}

U64* LookupTable::generateBlockerMusks(U64 movementMusk) {
    vector<int> squareIndicies; // extract all '1' index from movementMusk
    U64 movementMuskCopy = movementMusk;
    while (movementMuskCopy != 0) {
        squareIndicies.emplace_back(popLSB(movementMuskCopy));
    }

    int totalPatterns = 1 << squareIndicies.size(); // 2^n
    U64* blockerMusks = new U64[totalPatterns];
    if (totalPatterns == 1) {
        throw logic_error("Attempted to generate a blocker key for an empty movement musk");
    }

    for (int patternIndex = 0; patternIndex < totalPatterns; patternIndex++) {
        blockerMusks[patternIndex] = 0x0ULL;
        for (size_t bitIndex = 0; bitIndex < squareIndicies.size(); bitIndex++) {
            int bit = (patternIndex >> bitIndex) & 1;
            blockerMusks[patternIndex] |= (U64)bit << squareIndicies[bitIndex];
        }
    }
    return blockerMusks;
}

U64 generateLegalMoves(int square, U64 blockerMusk, Piece piece) {
    U64 legalMoveMusk = 0x0ULL;
    vector<int> directions;
    switch (piece) {
        case Piece::BISHOP:
            directions = {NEGATIVE_DIAGONAL, POSITIVE_DIAGONAL, -NEGATIVE_DIAGONAL, -POSITIVE_DIAGONAL};
            break;
        case Piece::ROOK:
            directions = {VERTICAL, HORIZONTAL, -VERTICAL, -HORIZONTAL};
            break;
        case Piece::QUEEN:
            directions = {NEGATIVE_DIAGONAL, POSITIVE_DIAGONAL, -NEGATIVE_DIAGONAL, -POSITIVE_DIAGONAL, VERTICAL, HORIZONTAL, -VERTICAL, -HORIZONTAL};
            break;
        default:
            throw logic_error("Attempted to generate legal moves from blockers through a non-sliding piece");
    }
    for (int direction : directions) {
        int checkSquare = square;
        while (true) {
            if ((checkSquare + direction < 0) || (checkSquare + direction > 63) || (abs(((checkSquare + direction) % 8) - (checkSquare % 8)) > 1)) {
                break;
            } else {
                checkSquare += direction;
            }
            setBit(legalMoveMusk, checkSquare);
            if (getBit(blockerMusk, checkSquare)) {
                break;
            }
        }
    }
    return legalMoveMusk;
}

U64 generatePawnMoves(int square, U64 blockerMusk, U64 pawnMusk, bool white) {
    U64 legalMoveMusk = 0x0ULL;
    if (white) {
        if (!getBit(blockerMusk, square + VERTICAL)) {
            setBit(legalMoveMusk, square + VERTICAL);
            if (square / 8 == 1 && !getBit(blockerMusk, square + 2*VERTICAL)) {
                setBit(legalMoveMusk, square + 2*VERTICAL);
            }
        }
        if (getBit(blockerMusk, square + POSITIVE_DIAGONAL)) {
            setBit(legalMoveMusk, square + POSITIVE_DIAGONAL);
        }
        if (getBit(blockerMusk, square + NEGATIVE_DIAGONAL)) {
            setBit(legalMoveMusk, square + NEGATIVE_DIAGONAL);
        }
    } else {
        if (!getBit(blockerMusk, square - VERTICAL)) {
            setBit(legalMoveMusk, square - VERTICAL);
            if (square / 8 == 6 && !getBit(blockerMusk, square - 2*VERTICAL)) {
                setBit(legalMoveMusk, square - 2*VERTICAL);
            }
        }
        if (getBit(blockerMusk, square - POSITIVE_DIAGONAL)) {
            setBit(legalMoveMusk, square - POSITIVE_DIAGONAL);
        }
        if (getBit(blockerMusk, square - NEGATIVE_DIAGONAL)) {
            setBit(legalMoveMusk, square - NEGATIVE_DIAGONAL);
        }
    }
    return legalMoveMusk;
}

void LookupTable::mapBlockerKeys() {
    for (int i = 0; i < 64; i++) {
        whitePawnBlockerShifts[i] = 64 - __builtin_popcountll(whitePawnMusks[i]);
        blackPawnBlockerShifts[i] = 64 - __builtin_popcountll(blackPawnMusks[i]);
        bishopBlockerShifts[i] = 64 - __builtin_popcountll(bishopMusks[i]);
        rookBlockerShifts[i] = 64 - __builtin_popcountll(rookMusks[i]);
    }

    // store magic numbers
    ifstream readWhitePawnFile("whitePawnMagicNumbers.txt");
    for (int i = 0; i < 64; ++i) {
        readWhitePawnFile >> whitePawnMagicArray[i];
    }

    ifstream readBlackPawnFile("blackPawnMagicNumbers.txt");
    for (int i = 0; i < 64; ++i) {
        readBlackPawnFile >> blackPawnMagicArray[i];
    }

    ifstream readBoshopFile("bishopMagicNumbers.txt");
    for (int i = 0; i < 64; ++i) {
        readBoshopFile >> bishopMagicArray[i];
    }

    ifstream readRookFile("rookMagicNumbers.txt");
    for (int i = 0; i < 64; ++i) {
        readRookFile >> rookMagicArray[i];
    }

    // bishops
    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(bishopMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(bishopMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            pair<int, U64> key = make_pair(square, blockerMusks[blockerIndex]);
            bishopMoves[key] = generateLegalMoves(square, blockerMusks[blockerIndex], Piece::BISHOP);
        }
    }

    // rooks
    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(rookMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(rookMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            pair<int, U64> key = make_pair(square, blockerMusks[blockerIndex]);
            rookMoves[key] = generateLegalMoves(square, blockerMusks[blockerIndex], Piece::ROOK);
        }
    }

    // white pawns
    for (int square = 8; square < 56; square++) {
        U64* blockerMusks{generateBlockerMusks(whitePawnMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(whitePawnMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            pair<int, U64> key = make_pair(square, blockerMusks[blockerIndex]);
            whitePawnMoves[key] = generatePawnMoves(square, blockerMusks[blockerIndex], whitePawnMusks[square], true);
        }
    }

    // black pawns
    for (int square = 8; square < 56; square++) {
        U64* blockerMusks{generateBlockerMusks(blackPawnMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(blackPawnMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            pair<int, U64> key = make_pair(square, blockerMusks[blockerIndex]);
            blackPawnMoves[key] = generatePawnMoves(square, blockerMusks[blockerIndex], blackPawnMusks[square], false);
        }
    }
}

