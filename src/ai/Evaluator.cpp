#include "Evaluator.h"

#include "src/bitboard/Magic.h"
#include "src/bitboard/bit_tools.h"
#include "src/core/PositionUtil.h"
#include "src/core/types.h"

#include <cmath>

// the higher this weight, the closer we are to the endgame
float Evaluator::getEndgameWeight(Position &pos) const {
    static constexpr float MAX_NONPAWNS = 14.0f;

    int numNonPawnPieces = 0;
    for (Piece p : ALL_PIECES) {
        if (pieceToPT(p) != PAWN && pieceToPT(p) != KING) {
            numNonPawnPieces += getBitCount(pos.getPieceBB(p));
        }
    }

    return 1.0f - numNonPawnPieces / MAX_NONPAWNS;
}

Eval Evaluator::getPsqtEval(float endgameWeight, Piece p, Square sq) const {
    static constexpr float WEIGHT = 0.5f;

    const int eg = int(endgameWeight * 256);
    const int mg = 256 - eg;

    const Eval mid = MIDDLEGAME_PSQT[p][sq];
    const Eval end = ENDGAME_PSQT[p][sq];

    return WEIGHT * (mid * mg + end * eg) / 256;
}

Eval Evaluator::getKingDistanceEval(float endgameWeight, Position &pos) const {
    static constexpr float WEIGHT = 5.0f;

    // hard cutoff
    if (endgameWeight <= 0.6f) {
        return 0;
    }

    // doesn't matter whose turn it is to move; endgameWeight already captures this
    const Square kingSq1 = pos.getKingSquare(WHITE), kingSq2 = pos.getKingSquare(BLACK);
    const int width = std::abs(fileOf(kingSq1) - fileOf(kingSq2));
    const int height = std::abs(rankOf(kingSq1) - rankOf(kingSq2));

    // this is the minimum number of king moves to move from one king to the other
    const int kingDist = std::max(width, height);

    // in endgames, favor moving the kings towards each other
    // note that the max distance on a 8x8 board is 7
    return WEIGHT * (7 - kingDist);
}

// the higher the score, the better for position is for pos.getSideToMove()
// and vice versa
Eval Evaluator::run(Position &pos) const {
    Eval totalEval = 0;
    Color side = pos.getSideToMove();
    float endgameWeight = getEndgameWeight(pos);

    ////////// START PIECE SCORING BLOCK //////////

    // reused often
    Bitboard whitePawns = pos.getPieceBB(WP);
    Bitboard blackPawns = pos.getPieceBB(BP);

    // WP
    forEachSquare(whitePawns, [&](Square sq) {
        // material value + PSQT bonus
        totalEval += (ptValues[PAWN] + getPsqtEval(endgameWeight, WP, sq));

        // passed pawn bonus
        if ((PASSED_PAWN_BLOCKER_MASKS[WHITE][sq] & blackPawns) == 0) {
            totalEval += PASSED_PAWN_BONUS;

            // pawn-protected passed pawn bonus
            if (PAWN_ATTACK_MASKS[BLACK][sq] & whitePawns) {
                totalEval += PROTECTED_PASSED_PAWN_BONUS;
            }
        }

        // doubled pawns penalty
        int numPawnsOnFile = getBitCount(FILE_MASKS[fileOf(sq)] & whitePawns);
        if (numPawnsOnFile > 1) {
            totalEval -= (numPawnsOnFile * DOUBLED_PAWNS_PENALTY);
        }
    });

    // WN
    forEachSquare(pos.getPieceBB(WN), [&](Square sq) {
        // material value + PSQT bonus
        totalEval += (ptValues[KNIGHT] + getPsqtEval(endgameWeight, WN, sq));
    });

    // WB
    forEachSquare(pos.getPieceBB(WB), [&](Square sq) {
        // material value + PSQT bonus
        totalEval += (ptValues[BISHOP] + getPsqtEval(endgameWeight, WB, sq));
    });

    // WR
    forEachSquare(pos.getPieceBB(WR), [&](Square sq) {
        // material value + PSQT bonus
        totalEval += (ptValues[ROOK] + getPsqtEval(endgameWeight, WR, sq));

        // semi-open file bonus
        if ((FILE_MASKS[fileOf(sq)] & blackPawns) == 0) {
            totalEval += SEMI_OPEN_FILE_BONUS;

            // open file bonus
            if ((FILE_MASKS[fileOf(sq)] & (whitePawns | blackPawns)) == 0) {
                totalEval += OPEN_FILE_BONUS;
            }
        }
    });

    // WQ
    forEachSquare(pos.getPieceBB(WQ), [&](Square sq) {
        // material value + PSQT bonus
        totalEval += (ptValues[QUEEN] + getPsqtEval(endgameWeight, WQ, sq));
    });

    // WK
    forEachSquare(pos.getPieceBB(WK), [&](Square sq) {
        // material value + PSQT bonus
        totalEval += (ptValues[KING] + getPsqtEval(endgameWeight, WK, sq));
    });

    // BP
    forEachSquare(blackPawns, [&](Square sq) {
        // material value + PSQT bonus
        totalEval -= (ptValues[PAWN] + getPsqtEval(endgameWeight, BP, sq));

        // passed pawn bonus
        if ((PASSED_PAWN_BLOCKER_MASKS[BLACK][sq] & whitePawns) == 0) {
            totalEval -= PASSED_PAWN_BONUS;

            // pawn-protected passed pawn bonus
            if (PAWN_ATTACK_MASKS[WHITE][sq] & blackPawns) {
                totalEval -= PROTECTED_PASSED_PAWN_BONUS;
            }
        }

        // doubled pawns penalty
        int numPawnsOnFile = getBitCount(FILE_MASKS[fileOf(sq)] & whitePawns);
        if (numPawnsOnFile > 1) {
            totalEval += (numPawnsOnFile * DOUBLED_PAWNS_PENALTY);
        }
    });

    // BN
    forEachSquare(pos.getPieceBB(BN), [&](Square sq) {
        // material value + PSQT bonus
        totalEval -= (ptValues[KNIGHT] + getPsqtEval(endgameWeight, BN, sq));
    });

    // BB
    forEachSquare(pos.getPieceBB(BB), [&](Square sq) {
        // material value + PSQT bonus
        totalEval -= (ptValues[BISHOP] + getPsqtEval(endgameWeight, BB, sq));
    });

    // BR
    forEachSquare(pos.getPieceBB(BR), [&](Square sq) {
        // material value + PSQT bonus
        totalEval -= (ptValues[ROOK] + getPsqtEval(endgameWeight, BR, sq));

        // semi-open file bonus
        if ((FILE_MASKS[fileOf(sq)] & whitePawns) == 0) {
            totalEval -= SEMI_OPEN_FILE_BONUS;

            // open file bonus
            if ((FILE_MASKS[fileOf(sq)] & (whitePawns | blackPawns)) == 0) {
                totalEval -= OPEN_FILE_BONUS;
            }
        }
    });

    // BQ
    forEachSquare(pos.getPieceBB(BQ), [&](Square sq) {
        // material value + PSQT bonus
        totalEval -= (ptValues[QUEEN] + getPsqtEval(endgameWeight, BQ, sq));
    });

    // BK
    forEachSquare(pos.getPieceBB(BK), [&](Square sq) {
        // material value + PSQT bonus
        totalEval -= (ptValues[KING] + getPsqtEval(endgameWeight, BK, sq));
    });

    ////////// END PIECE SCORING BLOCK //////////

    // king distance eval
    totalEval += getKingDistanceEval(endgameWeight, pos);

    // branchless way of returning -eval for black and +eval for white
    return side * -totalEval + (1 - side) * totalEval;
}
