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
int LookupTable::knightBlockerShifts[64];
U64 LookupTable::whitePawnMagicArray[64];
U64 LookupTable::blackPawnMagicArray[64];
U64 LookupTable::bishopMagicArray[64];
U64 LookupTable::rookMagicArray[64];
U64 LookupTable::knightPSTMagicArray[64];
U64 LookupTable::rookMagicMoves[64][4096];
U64 LookupTable::bishopMagicMoves[64][512];
U64 LookupTable::whitePawnMagicMoves[64][16];
U64 LookupTable::blackPawnMagicMoves[64][16];
double LookupTable::knightPSTTable[64][256];
double LookupTable::rookPSTTable[64][4096];
double LookupTable::bishopPSTTable[64][512];
U64 LookupTable::whitePawnPassMusk[64];
U64 LookupTable::blackPawnPassMusk[64];
double LookupTable::pawnPST[5] = {0, 0.1, 0.2, 0.3, 0.4};
double LookupTable::passPawnPST[5] = {0.2, 0.4, 0.6, 0.8, 1};

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
        case (Piece::KING):
            return kingMusks[key];
        case (Piece::QUEEN):
            return queenMusks[key];
        case (Piece::BISHOP):
            return bishopMusks[key];
        case (Piece::ROOK):
            return rookMusks[key];
        default:
            throw logic_error("Attempted to search an invalid musk");
    }
}

U64 LookupTable::lookupMove(int square, Piece piece, U64 allPieces) {
    switch (piece) {
        case (Piece::WHITEPAWN): {
            U64 blockers = whitePawnMusks[square] & allPieces;
            int blockerKey = (blockers * whitePawnMagicArray[square]) >> whitePawnBlockerShifts[square];
            return whitePawnMagicMoves[square][blockerKey];
        }
        case (Piece::BLACKPAWN): {
            U64 blockers = blackPawnMusks[square] & allPieces;
            int blockerKey = (blockers * blackPawnMagicArray[square]) >> blackPawnBlockerShifts[square];
            return blackPawnMagicMoves[square][blockerKey];
        }
        case (Piece::BISHOP): {
            U64 blockers = bishopMusks[square] & allPieces;
            int blockerKey = (blockers * bishopMagicArray[square]) >> bishopBlockerShifts[square];
            return bishopMagicMoves[square][blockerKey];
        }
        case (Piece::ROOK): {
            U64 blockers = rookMusks[square] & allPieces;
            int blockerKey = (blockers * rookMagicArray[square]) >> rookBlockerShifts[square];
            return rookMagicMoves[square][blockerKey];
        }
        case (Piece::QUEEN): {
            U64 blockersB = bishopMusks[square] & allPieces;
            int blockerKeyB = (blockersB * bishopMagicArray[square]) >> bishopBlockerShifts[square];
            U64 blockersR = rookMusks[square] & allPieces;
            int blockerKeyR = (blockersR * rookMagicArray[square]) >> rookBlockerShifts[square];
            return bishopMagicMoves[square][blockerKeyB] | rookMagicMoves[square][blockerKeyR];
        }
        default:
            throw logic_error("Attempted to search an invalid move");
    }
}

double LookupTable::lookupPawnPSTValue(int square, U64 opponentPawnMusk, Colour colour) {
    if (colour == Colour::WHITE) {
        int advanceIndex = (square / 8) - 1;
        if (advanceIndex >= 5) {
            return 3;
        } else {
            U64 passPawnMusk = whitePawnPassMusk[square];
            if ((passPawnMusk & opponentPawnMusk) != 0) {
                return pawnPST[advanceIndex];
            } else {
                return passPawnPST[advanceIndex];
            }
        }
    } else {
        int advanceIndex = (7 - (square / 8)) - 1;
        if (advanceIndex >= 5) {
            return 3;
        } else {
            U64 passPawnMusk = blackPawnPassMusk[square];
            if ((passPawnMusk & opponentPawnMusk) != 0) {
                return pawnPST[advanceIndex];
            } else {
                return passPawnPST[advanceIndex];
            }
        }
    }
}

double LookupTable::lookupKnightPSTValue(int square, U64 restrictedMobility) {
    U64 blockers = knightMusks[square] & restrictedMobility;
    int blockerKey = (blockers * knightPSTMagicArray[square]) >> knightBlockerShifts[square];
    return knightPSTTable[square][blockerKey];
}

double LookupTable::lookupPSTValue(int square, Piece piece, U64 restrictedMobility) {
    switch (piece) {
        case (Piece::BISHOP): {
            U64 blockers = bishopMusks[square] & restrictedMobility;
            int blockerKey = (blockers * bishopMagicArray[square]) >> bishopBlockerShifts[square];
            return bishopPSTTable[square][blockerKey] * 0.01;
        }
        case (Piece::ROOK): {
            U64 blockers = rookMusks[square] & restrictedMobility;
            int blockerKey = (blockers * rookMagicArray[square]) >> rookBlockerShifts[square];
            return rookPSTTable[square][blockerKey] * 0.01;
        }
        case (Piece::QUEEN): {
            U64 blockersB = bishopMusks[square] & restrictedMobility;
            int blockerKeyB = (blockersB * bishopMagicArray[square]) >> bishopBlockerShifts[square];
            U64 blockersR = rookMusks[square] & restrictedMobility;
            int blockerKeyR = (blockersR * rookMagicArray[square]) >> rookBlockerShifts[square];
            return (bishopPSTTable[square][blockerKeyB] + rookPSTTable[square][blockerKeyR]) * 0.004;
        }
        default:
            throw logic_error("Attempted to search pst value for an invalid piece");
    }
}

U64 LookupTable::lookupRayMusk(int square, int direction) {
    switch (direction) {
        case (VERTICAL): {
            return rayMusks[UPINDEX][square];
            break;
        }
        case (-VERTICAL): {
            return rayMusks[DOWNINDEX][square];
            break;
        }
        case (HORIZONTAL): {
            return rayMusks[RIGHTINDEX][square];
            break;
        }
        case (-HORIZONTAL): {
            return rayMusks[LEFTINDEX][square];
            break;
        }
        case (POSITIVE_DIAGONAL): {
            return rayMusks[UPRIGHTINDEX][square];
            break;
        }
        case (-POSITIVE_DIAGONAL): {
            return rayMusks[DOWNLEFTINDEX][square];
            break;
        }
        case (NEGATIVE_DIAGONAL): {
            return rayMusks[UPLEFTINDEX][square];
            break;
        }
        case (-NEGATIVE_DIAGONAL): {
            return rayMusks[DOWNRIGHTINDEX][square];
            break;
        }
        default:
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
        kingMusks[i] = (((fromSquare << NEGATIVE_DIAGONAL) | (fromSquare >> HORIZONTAL) | (fromSquare >> POSITIVE_DIAGONAL)) & ~H_FILE) | (((fromSquare >> NEGATIVE_DIAGONAL) | (fromSquare << HORIZONTAL) | (fromSquare << POSITIVE_DIAGONAL)) & ~A_FILE) | (fromSquare << VERTICAL | fromSquare >> VERTICAL);

        knightMusks[i] = (((fromSquare >> 6) | (fromSquare << 10)) & ~A_FILE & ~B_FILE) | (((fromSquare >> 10) | (fromSquare << 6)) & ~G_FILE & ~H_FILE) | (((fromSquare >> 15) | (fromSquare << 17)) & ~A_FILE) | (((fromSquare << 15) | (fromSquare >> 17)) & ~H_FILE);

        if (i >= 8 && i <= 55) {
            whitePawnMusks[i] = (((fromSquare << NEGATIVE_DIAGONAL)) & ~H_FILE) | (((fromSquare << POSITIVE_DIAGONAL)) & ~A_FILE) | (fromSquare << VERTICAL);
            if ((i / 8) == 1) {
                whitePawnMusks[i] |= (fromSquare << 2 * VERTICAL);
            }
        }

        if (i <= 55) {
            whitePawnControlMusks[i] = (((fromSquare << NEGATIVE_DIAGONAL)) & ~H_FILE) | (((fromSquare << POSITIVE_DIAGONAL)) & ~A_FILE);
        }

        if (i >= 8 && i <= 55) {
            blackPawnMusks[i] = (((fromSquare >> NEGATIVE_DIAGONAL)) & ~A_FILE) | (((fromSquare >> POSITIVE_DIAGONAL)) & ~H_FILE) | (fromSquare >> VERTICAL);
            if ((i / 8) == 6) {
                blackPawnMusks[i] |= (fromSquare >> 2 * VERTICAL);
            }
        }

        if (i >= 8) {
            blackPawnControlMusks[i] = (((fromSquare >> NEGATIVE_DIAGONAL)) & ~A_FILE) | (((fromSquare >> POSITIVE_DIAGONAL)) & ~H_FILE);
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
    vector<int> squareIndicies;  // extract all '1' index from movementMusk
    U64 movementMuskCopy = movementMusk;
    while (movementMuskCopy != 0) {
        squareIndicies.emplace_back(popLSB(movementMuskCopy));
    }

    int totalPatterns = 1 << squareIndicies.size();  // 2^n
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

double generatePSTValue(U64 moveMusk) {
    double pstValue = 0;
    pstValue += __builtin_popcountll(moveMusk & sevenSquares) * 7;
    pstValue += __builtin_popcountll(moveMusk & sixSquares) * 6;
    pstValue += __builtin_popcountll(moveMusk & fiveSquares) * 5;
    pstValue += __builtin_popcountll(moveMusk & fourSquares) * 4;
    pstValue += __builtin_popcountll(moveMusk & threeSquares) * 3;
    pstValue += __builtin_popcountll(moveMusk & twoSquares) * 2;
    pstValue += __builtin_popcountll(moveMusk & oneSquares) * 1;
    return pstValue;
}

U64 generatePawnMoves(int square, U64 blockerMusk, bool white) {
    U64 legalMoveMusk = 0x0ULL;
    if (white) {
        if (!getBit(blockerMusk, square + VERTICAL)) {
            setBit(legalMoveMusk, square + VERTICAL);
            if (square / 8 == 1 && !getBit(blockerMusk, square + 2 * VERTICAL)) {
                setBit(legalMoveMusk, square + 2 * VERTICAL);
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
            if (square / 8 == 6 && !getBit(blockerMusk, square - 2 * VERTICAL)) {
                setBit(legalMoveMusk, square - 2 * VERTICAL);
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
        if (i >= 8 && i <= 55) {
            whitePawnBlockerShifts[i] = 64 - __builtin_popcountll(whitePawnMusks[i]);
            blackPawnBlockerShifts[i] = 64 - __builtin_popcountll(blackPawnMusks[i]);
        }
        bishopBlockerShifts[i] = 64 - __builtin_popcountll(bishopMusks[i]);
        rookBlockerShifts[i] = 64 - __builtin_popcountll(rookMusks[i]);
        knightBlockerShifts[i] = 64 - __builtin_popcountll(knightMusks[i]);
    }

    // store magic numbers
    ifstream readWhitePawnFile("whitePawnMagicNumbers.txt");
    for (int i = 8; i <= 55; i++) {
        readWhitePawnFile >> whitePawnMagicArray[i];
    }

    ifstream readBlackPawnFile("blackPawnMagicNumbers.txt");
    for (int i = 8; i <= 55; i++) {
        readBlackPawnFile >> blackPawnMagicArray[i];
    }

    ifstream readBishopFile("bishopMagicNumbers.txt");
    for (int i = 0; i < 64; i++) {
        readBishopFile >> bishopMagicArray[i];
    }

    ifstream readRookFile("rookMagicNumbers.txt");
    for (int i = 0; i < 64; i++) {
        readRookFile >> rookMagicArray[i];
    }

    ifstream readKnightFile("knightPSTMagicNumbers.txt");
    for (int i = 0; i < 64; i++) {
        readKnightFile >> knightPSTMagicArray[i];
    }

    // bishops
    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(bishopMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(bishopMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            int blockerKey = (blockerMusks[blockerIndex] * bishopMagicArray[square]) >> bishopBlockerShifts[square];
            bishopMagicMoves[square][blockerKey] = generateLegalMoves(square, blockerMusks[blockerIndex], Piece::BISHOP);
            bishopPSTTable[square][blockerKey] = generatePSTValue(bishopMagicMoves[square][blockerKey] & ~blockerMusks[blockerIndex]);
        }
    }

    // rooks
    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(rookMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(rookMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            int blockerKey = (blockerMusks[blockerIndex] * rookMagicArray[square]) >> rookBlockerShifts[square];
            rookMagicMoves[square][blockerKey] = generateLegalMoves(square, blockerMusks[blockerIndex], Piece::ROOK);
            

            rookPSTTable[square][blockerKey] = generatePSTValue(rookMagicMoves[square][blockerKey] & ~blockerMusks[blockerIndex]);
        }
    }

    // white pawns
    for (int square = 8; square < 56; square++) {
        U64* blockerMusks{generateBlockerMusks(whitePawnMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(whitePawnMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            int blockerKey = (blockerMusks[blockerIndex] * whitePawnMagicArray[square]) >> whitePawnBlockerShifts[square];
            whitePawnMagicMoves[square][blockerKey] = generatePawnMoves(square, blockerMusks[blockerIndex], true);
        }
    }

    // black pawns
    for (int square = 8; square < 56; square++) {
        U64* blockerMusks{generateBlockerMusks(blackPawnMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(blackPawnMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            int blockerKey = (blockerMusks[blockerIndex] * blackPawnMagicArray[square]) >> blackPawnBlockerShifts[square];
            blackPawnMagicMoves[square][blockerKey] = generatePawnMoves(square, blockerMusks[blockerIndex], false);
        }
    }

    // knight
    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(knightMusks[square])};
        int totalPatterns = 1 << __builtin_popcountll(knightMusks[square]);
        for (int blockerIndex = 0; blockerIndex < totalPatterns; blockerIndex++) {
            int blockerKey = (blockerMusks[blockerIndex] * knightPSTMagicArray[square]) >> knightBlockerShifts[square];
            double pstValue = 0;
            U64 openSquares = knightMusks[square] & ~blockerMusks[blockerIndex];
            pstValue += __builtin_popcountll(openSquares & sevenSquares) * 7 * 0.012;
            pstValue += __builtin_popcountll(openSquares & sixSquares) * 6 * 0.012;
            pstValue += __builtin_popcountll(openSquares & fiveSquares) * 5 * 0.012;
            pstValue += __builtin_popcountll(openSquares & fourSquares) * 4 * 0.012;
            pstValue += __builtin_popcountll(openSquares & threeSquares) * 3 * 0.012;
            pstValue += __builtin_popcountll(openSquares & twoSquares) * 2 * 0.012;
            pstValue += __builtin_popcountll(openSquares & oneSquares) * 1 * 0.012;
            knightPSTTable[square][blockerKey] = pstValue;
        }
    }
}

void LookupTable::generateWhitePawnMagicNumbers() {
    for (int i = 8; i <= 55; i++) {
        whitePawnBlockerShifts[i] = 64 - __builtin_popcountll(whitePawnMusks[i]);
    }
    for (int square = 8; square <= 55; square++) {
        U64* blockerMusks{generateBlockerMusks(whitePawnMusks[square])};
        int numOfBits = __builtin_popcountll(whitePawnMusks[square]);
        int totalPatterns = 1 << numOfBits;
        unordered_set<U64> uniqueProducts;
        U64 testMagicNumber = generateRandomU64Exact(4);
        while (true) {
            bool unique = true;
            for (int i = 0; i < totalPatterns; i++) {
                U64 product = (blockerMusks[i] * testMagicNumber) >> whitePawnBlockerShifts[square];
                if (!(uniqueProducts.insert(product)).second) {
                    testMagicNumber = generateRandomU64Exact(4);
                    uniqueProducts.clear();
                    unique = false;
                    break;
                }
            }
            if (unique) {
                break;
            }
        }
    }
}

void LookupTable::generateBlackPawnMagicNumbers() {
    for (int i = 8; i <= 55; i++) {
        blackPawnBlockerShifts[i] = 64 - __builtin_popcountll(blackPawnMusks[i]);
    }
    for (int square = 8; square <= 55; square++) {
        U64* blockerMusks{generateBlockerMusks(blackPawnMusks[square])};
        int numOfBits = __builtin_popcountll(blackPawnMusks[square]);
        int totalPatterns = 1 << numOfBits;
        unordered_set<U64> uniqueProducts;
        U64 testMagicNumber = generateRandomU64Exact(4);
        while (true) {
            bool unique = true;
            for (int i = 0; i < totalPatterns; i++) {
                U64 product = (blockerMusks[i] * testMagicNumber) >> blackPawnBlockerShifts[square];
                if (!(uniqueProducts.insert(product)).second) {
                    testMagicNumber = generateRandomU64Exact(4);
                    uniqueProducts.clear();
                    unique = false;
                    break;
                }
            }
            if (unique) {
                break;
            }
        }
        cout << testMagicNumber << endl;
    }
}

void LookupTable::generateBishopMagicNumbers() {
    for (int i = 0; i < 64; i++) {
        bishopBlockerShifts[i] = 64 - __builtin_popcountll(bishopMusks[i]);
    }

    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(bishopMusks[square])};
        int numOfBits = __builtin_popcountll(bishopMusks[square]);
        int totalPatterns = 1 << numOfBits;
        unordered_set<U64> uniqueProducts;
        U64 testMagicNumber = generateRandomU64Exact(8);
        while (true) {
            bool unique = true;
            for (int i = 0; i < totalPatterns; i++) {
                U64 product = (blockerMusks[i] * testMagicNumber) >> bishopBlockerShifts[square];
                if (!(uniqueProducts.insert(product)).second) {
                    testMagicNumber = generateRandomU64Exact(8);
                    uniqueProducts.clear();
                    unique = false;
                    break;
                }
            }
            if (unique) {
                break;
            }
        }
        cout << testMagicNumber << endl;
    }
}

void LookupTable::generateRookMagicNumbers() {
    for (int i = 0; i < 64; i++) {
        rookBlockerShifts[i] = 64 - __builtin_popcountll(rookMusks[i]);
    }

    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(rookMusks[square])};
        int numOfBits = __builtin_popcountll(rookMusks[square]);
        int totalPatterns = 1 << numOfBits;
        unordered_set<U64> uniqueProducts;
        U64 testMagicNumber = generateRandomU64Range(6, 14);
        while (true) {
            bool unique = true;
            for (int i = 0; i < totalPatterns; i++) {
                U64 product = (blockerMusks[i] * testMagicNumber) >> rookBlockerShifts[square];
                if (!(uniqueProducts.insert(product)).second) {
                    testMagicNumber = generateRandomU64Range(6, 14);
                    uniqueProducts.clear();
                    unique = false;
                    break;
                }
            }
            if (unique) {
                break;
            }
        }
        cout << testMagicNumber << endl;
    }
}

void LookupTable::generateKnightMagicNumbers() {
    for (int i = 0; i < 64; i++) {
        knightBlockerShifts[i] = 64 - __builtin_popcountll(knightMusks[i]);
    }

    for (int square = 0; square < 64; square++) {
        U64* blockerMusks{generateBlockerMusks(knightMusks[square])};
        int numOfBits = __builtin_popcountll(knightMusks[square]);
        int totalPatterns = 1 << numOfBits;
        unordered_set<U64> uniqueProducts;
        U64 testMagicNumber = generateRandomU64Range(5, 9);
        while (true) {
            bool unique = true;
            for (int i = 0; i < totalPatterns; i++) {
                U64 product = (blockerMusks[i] * testMagicNumber) >> knightBlockerShifts[square];
                if (!(uniqueProducts.insert(product)).second) {
                    testMagicNumber = generateRandomU64Range(5, 9);
                    uniqueProducts.clear();
                    unique = false;
                    break;
                }
            }
            if (unique) {
                break;
            }
        }
        cout << testMagicNumber << endl;
    }
}

void LookupTable::generatePawnPassMusk() {
    for (int square = 8; square <= 55; square++) {
        whitePawnPassMusk[square] = 0x0ULL;
        blackPawnPassMusk[square] = 0x0ULL;

        whitePawnPassMusk[square] |= lookupRayMusk(square, VERTICAL);
        blackPawnPassMusk[square] |= lookupRayMusk(square, -VERTICAL);

        if (!getBit(A_FILE, square)) {
            int sideSquare = square - 1;
            whitePawnPassMusk[square] |= lookupRayMusk(sideSquare, VERTICAL);
            blackPawnPassMusk[square] |= lookupRayMusk(sideSquare, -VERTICAL);
        }

        if (!getBit(H_FILE, square)) {
            int sideSquare = square + 1;
            whitePawnPassMusk[square] |= lookupRayMusk(sideSquare, VERTICAL);
            blackPawnPassMusk[square] |= lookupRayMusk(sideSquare, -VERTICAL);
        }
    }
}

void LookupTable::printStuff() {
    for (int i = 0; i < 64; i++) {
        cout << i << endl;
        printBitboard(whitePawnPassMusk[i], cout);
        printBitboard(blackPawnPassMusk[i], cout);
    }
};
