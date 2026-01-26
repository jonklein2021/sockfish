#include "Evaluator.h"

#include "src/bitboard/bit_tools.h"
#include "src/core/PositionUtil.h"
#include "src/core/types.h"

#include <cmath>

// the higher this weight, the closer we are to the endgame
float Evaluator::getEndgameWeight(Position &pos) const {
    // each side has 7 pieces that aren't pawns nor their king at the start
    constexpr float ONE_SEVENTH = 1 / 7.0;

    // only count for the opponent
    int numNonPawnPieces = 0;
    Color opponent = otherColor(pos.getSideToMove());
    for (Piece p : COLOR_TO_PIECES[opponent]) {
        numNonPawnPieces += getBitCount(pos.getPieceBB(p));
    }

    return 1 - ONE_SEVENTH * numNonPawnPieces;
}

Eval Evaluator::getPSTableEval(float endgameWeight, Piece p, Square sq) const {
    constexpr float WEIGHT = 0.1;

    // squaring helps hold back heavy weight until there truly are very few pieces remaining
    const float egSquareWeight = endgameWeight * endgameWeight;
    const Eval standardPSValue = (1 - egSquareWeight) * pieceSqTables[p][sq];
    const Eval endgamePSValue = egSquareWeight * endgamePieceSqTables[p][sq];
    return WEIGHT * (standardPSValue + endgamePSValue);
}

Eval Evaluator::getKingDistanceEval(float endgameWeight, Position &pos) const {
    constexpr float WEIGHT = 10.0;

    // doesn't matter whose turn it is to move; endgameWeight already captures this
    const Square kingSq1 = pos.getKingSquare(WHITE), kingSq2 = pos.getKingSquare(BLACK);
    const int fileDiff = std::abs(fileOf(kingSq1) - fileOf(kingSq2)),
              rankDiff = std::abs(rankOf(kingSq1) - rankOf(kingSq2));

    const int manhattanKingDist = fileDiff + rankDiff;

    // in endgames, favor moving the kings towards each other
    // note that the max manhattan distance on a 8x8 board is 14
    return WEIGHT * endgameWeight * (14 - manhattanKingDist);
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
    return SIGN[pos.getSideToMove()] * totalEval;
}
