#include "MoveGenerator.h"

#include "Move.h"
#include "MoveComputers.h"
#include "Position.h"
#include "bit_tools.h"
#include "src/types.h"

MoveGenerator::MoveGenerator(std::shared_ptr<Position> _pos)
    : pos(_pos) {}

/**
 * Returns return if and only if the given square is under attack by the given
 * side
 */
constexpr bool MoveGenerator::underAttack(Square sq, Color side) {
    // return (1 << sq) & (white ? (pieceAttacks[WP] | pieceAttacks[WN] | pieceAttacks[WB] |
    //                              pieceAttacks[WR] | pieceAttacks[WQ] | pieceAttacks[WK])
    //                           : (pieceAttacks[BP] | pieceAttacks[BN] | pieceAttacks[BB] |
    //                              pieceAttacks[BR] | pieceAttacks[BQ] | pieceAttacks[BK]));
    // TODO: This
    return false;
}

/**
 * Returns return if and only if the given square is under attack by the
 * opponent
 * @param
 * squareBit The square to check
 */
constexpr bool MoveGenerator::underAttack(Square sq) {
    return underAttack(sq, otherColor(pos->getSideToMove()));
}

constexpr bool MoveGenerator::insufficientMaterial() {
    // count all pieces
    auto board = pos->board;
    const int pieceCount[12] = {
        __builtin_popcountll(board.getPieces(WP)),
        __builtin_popcountll(board.getPieces(WN)),
        __builtin_popcountll(board.getPieces(WB)),
        __builtin_popcountll(board.getPieces(WR)),
        __builtin_popcountll(board.getPieces(WQ)),
        1,  // always 1 white king
        __builtin_popcountll(board.getPieces(BP)),
        __builtin_popcountll(board.getPieces(BN)),
        __builtin_popcountll(board.getPieces(BB)),
        __builtin_popcountll(board.getPieces(BR)),
        __builtin_popcountll(board.getPieces(BQ)),
        1  // always 1 black king
    };

    // note: consider testing for king + two knights vs king

    // insufficient material if at most one knight or bishop is left per side
    return !(pieceCount[WN] & pieceCount[WB]) & !(pieceCount[BN] & pieceCount[BB]) &
           !(pieceCount[WP] | pieceCount[WR] | pieceCount[WQ] | pieceCount[BP] | pieceCount[BR] |
             pieceCount[BQ]);
}

constexpr bool MoveGenerator::isTerminal() {
    // TODO: This
    return false;
}

constexpr bool MoveGenerator::isCheck() {
    // get position of king
    const Bitboard kingBB = pos->board.getPieces(pos->getSideToMove() == WHITE ? WK : BK);
    const Square kingSq = Square(indexOfLs1b(kingBB));

    // check if king is under attack
    return underAttack(kingSq);
}

constexpr bool MoveGenerator::isMoveLegal(Position &copy, Square kingSq, Move &move, Color side) {
    // simulate move on copied state
    Position::Metadata md = copy.makeMove(move);

    // test if king is in check after move
    bool isOurKingInCheck = underAttack(kingSq, otherColor(side));

    // unmake move to preserve original state
    copy.unmakeMove(move, md);

    // return true iff the move is not illegal
    return !isOurKingInCheck;
}

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
    const Color toMove = pos->getSideToMove();

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
