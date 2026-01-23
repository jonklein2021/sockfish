#include "MoveGenerator.h"

#include "MoveComputers.h"
#include "src/core/Move.h"
#include "src/core/Position.h"
#include "src/core/types.h"

#include <algorithm>

template<Color side>
void MoveGenerator::generatePseudolegal(std::vector<Move> &result, Position &pos) {
    /* PAWNS */
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnPushes<side>);
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnCaptures<side>);
    appendMovesFromPiece<PAWN, Move::EN_PASSANT>(result, pos,
                                                 MoveComputers::computePawnEnPassant<side>);

    /* KNIGHTS */
    appendMovesFromPiece<KNIGHT, Move::NORMAL>(result, pos,
                                               MoveComputers::computeKnightMoves<side>);

    /* BISHOPS */
    appendMovesFromPiece<BISHOP, Move::NORMAL>(result, pos,
                                               MoveComputers::computeBishopMoves<side>);

    /* ROOKS */
    appendMovesFromPiece<ROOK, Move::NORMAL>(result, pos, MoveComputers::computeRookMoves<side>);

    /* QUEENS */
    appendMovesFromPiece<QUEEN, Move::NORMAL>(result, pos, MoveComputers::computeQueenMoves<side>);

    /* KING */
    appendMovesFromPiece<KING, Move::NORMAL>(result, pos, MoveComputers::computeKingMoves<side>);

    /* CASTLING */
    appendCastlingMoves(result, pos);
}

void MoveGenerator::generatePseudolegal(std::vector<Move> &result, Position &pos) {
    if (pos.getSideToMove() == WHITE) {
        generatePseudolegal<WHITE>(result, pos);
    } else {
        generatePseudolegal<BLACK>(result, pos);
    }
}

void MoveGenerator::generateLegal(std::vector<Move> &result, Position &pos) {
    result.clear();
    std::vector<Move> pseudolegal;
    generatePseudolegal(pseudolegal, pos);

    // only copy non-violating moves to legal vector
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
                 [&](Move m) { return isMoveLegal(pos, m); });
}

// used for quiescence search
void MoveGenerator::generatePseudolegalCaptures(std::vector<Move> &result, Position &pos) {
    result.clear();
    std::vector<Move> pseudolegal;
    generatePseudolegal(pseudolegal, pos);

    // only copy legal captures
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
                 [&](Move m) { return isCaptureMove(pos, m); });
}
