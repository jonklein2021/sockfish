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

// TODO
void MoveGenerator::generateSingleCheckEvasions(std::vector<Move> &result, Position &pos) {
    const Color toMove = pos.getSideToMove();
    const Square kingSq = pos.getKingSquare(toMove);

    // Option 1. Move the king
    const Bitboard kingMoves = MoveComputers::computeKingMoves(pos, kingSq);
    appendMovesFromBitboard<Move::NORMAL>(result, kingMoves, kingSq);

    // 2. Capture the checking piece
    // 3. Block the check
}

void MoveGenerator::generateDoubleCheckEvasions(std::vector<Move> &result, Position &pos) {
    // double check: only king moves are possible
    const Color toMove = pos.getSideToMove();
    const Square kingSq = pos.getKingSquare(toMove);
    const Bitboard kingMoves = MoveComputers::computeKingMoves(pos, kingSq);
    appendMovesFromBitboard<Move::NORMAL>(result, kingMoves, kingSq);
}

void MoveGenerator::generateLegal(std::vector<Move> &result, Position &pos) {
    const Color toMove = pos.getSideToMove();
    const Color opponent = otherColor(toMove);

    // When in check, there are 3 options to consider
    // 1. Move the King
    // 2. Capture the piece giving check
    // 3. Block the check
    // Caveats:
    // a. If it is double check, only 1 is possible
    // b. 3 is impossible if a knight is giving check
    int numCheckers = pos.getNumAttackers(pos.getKingSquare(toMove), opponent);
    if (numCheckers == 1) {
        generateSingleCheckEvasions(result, pos);
        return;
    } else if (numCheckers > 1) {
        generateDoubleCheckEvasions(result, pos);
        return;
    }

    Board board = pos.getBoardCopy();

    // check for pins
    Bitboard pinnedPieces = pos.getPinnedPieces();
    while (pinnedPieces) {
        Square sq = Square(getLsbIndex(pinnedPieces));
        Piece p = pos.pieceAt(sq);

        // remove this from board to avoid double-counting its moves
        board.removePiece(p, sq);

        // determine pinned piece movement by computing a ray between the king and the piece
        const Bitboard connectingRay = findOverlapRay(sq, pos.getKingSquare(toMove));

        if (pieceToPT(p) == PAWN) {
            Bitboard pawnMoves = MoveComputers::computePawnPushes(pos, sq) |
                                 MoveComputers::computePawnCaptures(pos, sq);

            // restrict movement within the pin ray
            pawnMoves &= connectingRay;

            const Bitboard promotingMoves = pawnMoves & PROMOTING_RANKS[toMove];
            const Bitboard nonPromotingMoves = pawnMoves & ~PROMOTING_RANKS[toMove];

            // create and append moves
            appendMovesFromBitboard<Move::PROMOTION>(result, promotingMoves, sq);
            appendMovesFromBitboard<Move::NORMAL>(result, nonPromotingMoves, sq);

            // TODO: Check elsewhere that this doesn't reveal a lateral check
            const Bitboard epCaptureBB = MoveComputers::computePawnEnPassant(pos, sq);
            appendMovesFromBitboard<Move::EN_PASSANT>(result, epCaptureBB, sq);
        } else {
            const Bitboard normalMoves = MoveComputers::moveAttackComputers[p](pos, sq);
            const Bitboard restrictedMoves = normalMoves & connectingRay;
            appendMovesFromBitboard<Move::NORMAL>(result, restrictedMoves, sq);
        }

        pinnedPieces &= pinnedPieces - 1;
    }

    // since we aren't in check and there are no pinned pieces remaining, we can generate moves
    // normally

    /* PAWNS */
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnPushes);
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnCaptures);

    if (pos.getEpSquare() != NO_SQ) {
        appendMovesFromPiece<PAWN, Move::EN_PASSANT>(result, pos,
                                                     MoveComputers::computePawnEnPassant);
    }

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
//     result.clear();
//     std::vector<Move> pseudolegal;
//     generatePseudolegal(pseudolegal, pos);
//
//     // only copy non-violating moves to legal vector
//     std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
//                  [&](Move m) { return isMoveLegal(pos, m); });
// }

// used for quiescence search
void MoveGenerator::generateLegalCaptures(std::vector<Move> &result, Position &pos) {
    result.clear();
    std::vector<Move> pseudolegal;
    generatePseudolegal(pseudolegal, pos);

    // only copy legal captures
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
                 [&](Move m) { return isCaptureMove(pos, m) && isMoveLegal(pos, m); });
}
