#include "MoveGenerator.h"

#include "Move.h"
#include "MoveComputers.h"
#include "Position.h"
#include "bit_tools.h"
#include "src/types.h"

MoveGenerator::MoveGenerator(std::shared_ptr<Position> _pos)
    : pos(_pos) {}

void MoveGenerator::appendMovesFromBitboard(std::vector<Move> &moveList,
                                            Bitboard moves,
                                            Square srcSq) {
    while (moves) {
        const Bitboard destSqBB = moves & -moves;
        const Square destSq = Square(indexOfLs1b(destSqBB));

        moveList.emplace_back(srcSq, destSq);

        moves ^= destSqBB;
    }
}

template<typename MoveComputer>
void MoveGenerator::appendMovesFromPiece(std::vector<Move> &moveList,
                                         PieceType pt,
                                         MoveComputer moveComputer) {
    const Color toMove = pos->getSideToMove();
    auto bb = pos->board.getPieces(ptToPiece(pt, toMove));  // N.B: this needs to make a copy
    while (bb) {
        const Bitboard srcSqBB = bb & -bb;  // isolate the LSB
        const Square srcSq = Square(indexOfLs1b(srcSqBB));

        // compute a bitboard of all destination squares that this piece can go to,
        // according to the specified moveComputer
        const Bitboard computedDstBB = moveComputer(pos, srcSq);

        // create and append moves to the moveList
        appendMovesFromBitboard(moveList, computedDstBB, srcSq);

        // pop this bit
        bb ^= srcSqBB;
    }
}

// TODO: Finish this
std::vector<Move> MoveGenerator::generatePseudolegal() {
    // cannot do 218 max move space optimization since these are pseudolegal moves
    std::vector<Move> moveList;

    /* PAWNS */
    appendMovesFromPiece(moveList, PAWN, MoveComputers::computePawnMoves);
    appendMovesFromPiece(moveList, PAWN, MoveComputers::computePawnAttacks);

    /* KNIGHTS */
    appendMovesFromPiece(moveList, KNIGHT, MoveComputers::computeKnightAttacks);

    /* BISHOPS */
    appendMovesFromPiece(moveList, BISHOP, MoveComputers::computeBishopAttacks);

    /* ROOKS */
    appendMovesFromPiece(moveList, BISHOP, MoveComputers::computeBishopAttacks);

    /* QUEENS */
    appendMovesFromPiece(moveList, QUEEN, MoveComputers::computeQueenAttacks);

    /* KING */
    appendMovesFromPiece(moveList, KING, MoveComputers::computeKingAttacks);

    return moveList;
}

std::vector<Move> MoveGenerator::generateLegal() {
    // TODO: This
    return {};
}
