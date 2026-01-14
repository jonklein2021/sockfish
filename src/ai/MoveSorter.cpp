#include "MoveSorter.h"

#include <algorithm>

void MoveSorter::run(const Position &pos, std::vector<Move> &moveList) {
    std::sort(moveList.begin(), moveList.end(), [this, pos](const Move &a, const Move &b) {
        return rateMove(pos, a) > rateMove(pos, b);
    });
}

MoveScore MoveSorter::rateMove(const Position &pos, const Move &move) {
    Eval rating = 0;
    const PieceType movedPT = pieceToPT(pos.pieceAt(move.getFromSquare()));

    // MVV-LVA
    const Piece capturedPiece = pos.pieceAt(move.getToSquare());
    if (capturedPiece != NO_PIECE) {
        rating += pieceTypeValues[pieceToPT(capturedPiece)] * 10 - pieceTypeValues[movedPT];
    }

    // pawn promotion moves are likely to be good
    if (move.isPromotion()) {
        rating += 50 * pieceTypeValues[move.getPromotedPieceType()] - pieceTypeValues[movedPT];
    }

    // commented out because rateMove shouldn't change Position state
    // moves that put the opponent in check should also be checked early
    // Position::Metadata prevMD = pos.makeMove(move);
    // if (PositionUtil::isCheck(pos)) {
    //     rating += 10;
    // }
    // pos.unmakeMove(move, prevMD);

    return rating;
}
