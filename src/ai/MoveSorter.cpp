#include "MoveSorter.h"

#include <algorithm>

void MoveSorter::run(const Position &pos, MoveList &moveList) {
    std::sort(moveList.begin(), moveList.end(), [this, pos](const Move &a, const Move &b) {
        return rateMove(pos, a) > rateMove(pos, b);
    });
}

// TODO: rate killer moves highly
MoveScore MoveSorter::rateMove(const Position &pos, const Move &move) {
    Eval rating = 0;
    const PieceType movedPT = pieceToPT(pos.pieceAt(move.getFromSquare()));

    // MVV-LVA
    const Piece capturedPiece = pos.pieceAt(move.getToSquare());
    if (capturedPiece != NO_PIECE) {
        rating += 10 * pieceTypeValues[pieceToPT(capturedPiece)] - pieceTypeValues[movedPT];
    }

    // pawn promotion moves are likely to be good
    if (move.isPromotion()) {
        rating += 50 * pieceTypeValues[move.getPromotedPieceType()] - pieceTypeValues[movedPT];
    }

    return rating;
}
