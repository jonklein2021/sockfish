#include "Evaluator.h"

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

Eval Evaluator::getPSTableEval(float endgameWeight, Piece p, Square sq) const {
    const int eg = int(endgameWeight * 256);
    const int mg = 256 - eg;

    const Eval mid = pieceSqTables[p][sq];
    const Eval end = endgamePieceSqTables[p][sq];

    return (mid * mg + end * eg) / 256;
}

Eval Evaluator::getKingDistanceEval(float endgameWeight, Position &pos) const {
    static constexpr float WEIGHT = 10.0;

    // hard cutoff
    if (endgameWeight <= 0.6f) {
        return 0.0;
    }

    // doesn't matter whose turn it is to move; endgameWeight already captures this
    const Square kingSq1 = pos.getKingSquare(WHITE), kingSq2 = pos.getKingSquare(BLACK);
    const int fileDiff = std::abs(fileOf(kingSq1) - fileOf(kingSq2)),
              rankDiff = std::abs(rankOf(kingSq1) - rankOf(kingSq2));

    const int manhattanKingDist = fileDiff + rankDiff;

    // in endgames, favor moving the kings towards each other
    // note that the max manhattan distance on a 8x8 board is 14
    return WEIGHT * (14 - manhattanKingDist);
}

// the higher the score, the better for position is for pos.getSideToMove()
// and vice versa
Eval Evaluator::run(Position &pos) const {
    Eval totalEval = 0;

    // total up pieces, scaling by piece value and position in piece-square
    // board
    const float endgameWeight = getEndgameWeight(pos);
    for (Color c : COLORS) {
        for (Piece p : COLOR_TO_PIECES[c]) {
            forEachSquare(pos.getPieceBB(p), [&](Square sq) {
                const Eval materialEval = pieceTypeValues[pieceToPT(p)];
                const Eval psTableEval = getPSTableEval(endgameWeight, p, sq);
                const Eval pieceEval = materialEval + psTableEval;
                totalEval += SIGN[c] * pieceEval;
            });
        }
    }

    // king distance eval
    totalEval += getKingDistanceEval(endgameWeight, pos);

    // return score relative to the current player for negamax
    return pos.getSideToMove() == WHITE ? totalEval : -totalEval;
}
