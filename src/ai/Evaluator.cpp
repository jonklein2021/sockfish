#include "Evaluator.h"

#include "src/core/PositionUtil.h"

Eval Evaluator::run(Position &pos) {
    const int sign = SIGN[pos.getSideToMove()];
    if (PositionUtil::isCheckmate(pos)) {
        return sign * pieceTypeValues[KING];
    }

    // need to fine-tune this
    const float piecePositionWeight = 0.05;

    Eval score = 0;

    // total up pieces, scaling by piece value and position in piece-square
    // board
    for (Square sq : ALL_SQUARES) {
        const Piece p = pos.pieceAt(sq);
        if (p == NO_PIECE) {
            continue;
        }
        const Eval value =
            pieceTypeValues[pieceToPT(p)] + piecePositionWeight * pieceSquareTables[p][sq];
        const Color c = pieceColor(p);
        score += SIGN[c] * value;
    }

    // mobility bonus
    // score += 0.1 * legalMoves.size();

    // return score relative to the current player for negamax
    return sign * score;
}
