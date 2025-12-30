#include "MoveGenerator.h"

#include "Move.h"
#include "MoveComputers.h"
#include "Position.h"
#include "bit_tools.h"
#include "src/types.h"

#include <algorithm>

bool MoveGenerator::isMoveLegal(std::shared_ptr<Position> pos, Move &move) {
    return true;
    // TODO: FIX Position::makeMove and Position::unmakeMove() before uncommenting the rest
    // Color moveMaker = pos->getSideToMove();
    //
    // // simulate move on copied state
    // Position::Metadata md = pos->makeMove(move);
    //
    // // test if king is in check after move
    // bool isOurKingInCheck = PositionUtil::isCheck(pos, moveMaker);
    //
    // // unmake move to preserve original state
    // pos->unmakeMove(move, md);
    //
    // // return true iff the move hasn't put its own king in check
    // return !isOurKingInCheck;
}

void MoveGenerator::appendMovesFromBitboard(std::vector<Move> &moveList,
                                            Bitboard moves,
                                            Square srcSq) {
    while (moves) {
        const Bitboard destSqBB = moves & -moves;
        const Square destSq = Square(indexOfLs1b(destSqBB));

        // TODO: Encode special moves (promotions, en passant, castling)
        moveList.emplace_back(srcSq, destSq);

        moves ^= destSqBB;
    }
}

template<typename MoveComputer>
void MoveGenerator::appendMovesFromPiece(std::shared_ptr<Position> pos,
                                         std::vector<Move> &moveList,
                                         PieceType pt,
                                         MoveComputer moveComputer) {
    const Color toMove = pos->getSideToMove();
    const Piece piece = ptToPiece(pt, toMove);
    auto bb = pos->board.getPieces(piece);  // N.B: this needs to make a copy
    while (bb) {
        const Bitboard srcSqBB = bb & -bb;  // isolate the LSB
        const Square srcSq = Square(indexOfLs1b(srcSqBB));

        // compute a bitboard of all destination squares that this piece can go to,
        // according to the specified moveComputer
        Bitboard computedDstBB = moveComputer(pos, srcSq);

        // create and append moves to the moveList
        appendMovesFromBitboard(moveList, computedDstBB, srcSq);

        // pop this bit
        bb ^= srcSqBB;
    }
}

// TODO: Finish this
std::vector<Move> MoveGenerator::generatePseudolegal(std::shared_ptr<Position> pos) {
    // cannot do 218 max move space optimization since these are pseudolegal moves
    std::vector<Move> moveList;

    /* PAWNS */
    appendMovesFromPiece(pos, moveList, PAWN, MoveComputers::computePawnMoves);
    appendMovesFromPiece(pos, moveList, PAWN, MoveComputers::computePawnAttacks);

    /* KNIGHTS */
    appendMovesFromPiece(pos, moveList, KNIGHT, MoveComputers::computeKnightAttacks);

    /* BISHOPS */
    appendMovesFromPiece(pos, moveList, BISHOP, MoveComputers::computeBishopAttacks);

    /* ROOKS */
    appendMovesFromPiece(pos, moveList, BISHOP, MoveComputers::computeBishopAttacks);

    /* QUEENS */
    appendMovesFromPiece(pos, moveList, QUEEN, MoveComputers::computeQueenAttacks);

    /* KING */
    appendMovesFromPiece(pos, moveList, KING, MoveComputers::computeKingAttacks);

    return moveList;
}

std::vector<Move> MoveGenerator::generateLegal(std::shared_ptr<Position> pos) {
    std::vector<Move> pseudolegal = generatePseudolegal(pos);
    std::vector<Move> legal;
    legal.reserve(218);

    // only copy non-violating moves to legal vector
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(legal),
                 [&](Move m) { return isMoveLegal(pos, m); });

    return legal;
}
