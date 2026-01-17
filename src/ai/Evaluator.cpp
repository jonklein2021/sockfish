#include "Evaluator.h"

#include "src/core/PositionUtil.h"
#include "src/movegen/MoveGenerator.h"

Eval Evaluator::run(Position &pos) {
    std::vector<Move> legalMoves;
    MoveGenerator::generateLegal(legalMoves, pos);
    return run(pos, legalMoves);
}

// the higher the score, the better for position is for pos.getSideToMove()
// and vice versa
Eval Evaluator::run(Position &pos, std::vector<Move> &legalMoves) {
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
        score += SIGN[pieceColor(p)] * value;
    }

    // mobility bonus
    score += mobilityBonusWeight * legalMoves.size();

    // return score relative to the current player for negamax
    return SIGN[pos.getSideToMove()] * score;
}
