#include "MoveGenerator.h"

#include "MoveComputers.h"
#include "src/core/Move.h"
#include "src/core/Position.h"
#include "src/core/types.h"

#include <algorithm>

void MoveGenerator::generatePseudolegal(std::vector<Move> &result, Position &pos) {
    // cannot do 218 max move space optimization since these are pseudolegal moves
    pos.board.updateOccupancies();

    /* PAWNS */
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnPushes);
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnCaptures);
    appendMovesFromPiece<PAWN, Move::EN_PASSANT>(result, pos, MoveComputers::computePawnEnPassant);

    /* KNIGHTS */
    appendMovesFromPiece<KNIGHT, Move::NORMAL>(result, pos, MoveComputers::computeKnightMoves);

    /* BISHOPS */
    appendMovesFromPiece<BISHOP, Move::NORMAL>(result, pos, MoveComputers::computeBishopMoves);

    /* ROOKS */
    appendMovesFromPiece<ROOK, Move::NORMAL>(result, pos, MoveComputers::computeRookMoves);

    /* QUEENS */
    appendMovesFromPiece<QUEEN, Move::NORMAL>(result, pos, MoveComputers::computeQueenMoves);

    /* KING */
    appendMovesFromPiece<KING, Move::NORMAL>(result, pos, MoveComputers::computeKingMoves);

    /* CASTLING */
    appendCastlingMoves(result, pos);
}

// void MoveGenerator::generateLegal(std::vector<Move> &result, Position &pos) {
//     Board board = pos.getBoardCopy();
//     const Color toMove = pos.getSideToMove();
//     if (PositionUtil::isCheck(pos)) {
//         // When in check, you can either
//         // 1. Move the King
//         // 2. Capture the piece giving check
//         // 3. Block the check
//         // Caveats:
//         // a. If it is double check, only 1 is possible
//         // b. 3 is impossible if a knight is giving check
//         return;
//     }
//
//     // check for pins
//     Bitboard pinnedPieces = pos.getPinnedPieces();
//     while (pinnedPieces) {
//         Square sq = Square(getLsbIndex(pinnedPieces));
//         Piece pinnedPiece = pos.pieceAt(sq);
//
//         // remove this from board to avoid double-counting its moves
//         board.removePiece(pinnedPiece, sq);
//
//         // determine type of pin by computing a ray between the king and the pinned piece
//         const Bitboard raySqBB = findOverlapRay(sq, pos.getKingSquare(toMove));
//
//         // the intersection of this ray and the piece's normal moves are the allowed dst sqs
//
//         pinnedPieces &= pinnedPieces - 1;
//     }
// }

void MoveGenerator::generateLegal(std::vector<Move> &result, Position &pos) {
    result.clear();
    std::vector<Move> pseudolegal;
    generatePseudolegal(pseudolegal, pos);

    // only copy non-violating moves to legal vector
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
                 [&](Move m) { return isMoveLegal(pos, m); });
}

// used for quiescence search
void MoveGenerator::generateLegalCaptures(std::vector<Move> &result, Position &pos) {
    result.clear();
    std::vector<Move> pseudolegal;
    generatePseudolegal(pseudolegal, pos);

    // only copy legal captures
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
                 [&](Move m) { return isCaptureMove(pos, m) && isMoveLegal(pos, m); });
}
