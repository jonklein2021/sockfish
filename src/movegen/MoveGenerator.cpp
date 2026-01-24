#include "MoveGenerator.h"

#include "MoveComputers.h"
#include "src/bitboard/Magic.h"
#include "src/bitboard/bit_tools.h"
#include "src/core/Move.h"
#include "src/core/Position.h"
#include "src/core/types.h"

#include <algorithm>
#include <cassert>

namespace MoveGenerator {

// for reuse by MoveGen functions
static Bitboard friendlyPieces = 0ull;
static Bitboard enemyPieces = 0ull;
static Bitboard occupiedSquares = 0ull;
static Bitboard emptySquares = 0ull;

template<PieceType Pt, Color Side>
inline void generateSlidingPieceMoves(std::vector<Move> &result, Position &pos) {
    Bitboard bb = pos.getPieceBB(ptToPiece(Pt, Side));
    forEachSquare(bb, [&](Square srcSq) {
        Bitboard attacks =
            Magic::getSlidingPieceAttacks<Pt>(srcSq, occupiedSquares) & ~friendlyPieces;
        appendMovesFromBitboard<Move::NORMAL>(result, attacks, srcSq);
    });
}

template<Color Side>
inline void generateCastlingMoves(std::vector<Move> &moveList, Position &pos) {
    // for each of the following constexpr arrays, the 1st element is relevant for white, and the
    // 2nd for black. this helps avoid branching

    static constexpr std::array<CastleRights, 2> KINGSIDE = {WHITE_OO, BLACK_OO};
    static constexpr std::array<CastleRights, 2> QUEENSIDE = {WHITE_OOO, BLACK_OOO};

    static constexpr Bitboard EMPTY_K[2] = {(1ull << f1) | (1ull << g1),
                                            (1ull << f8) | (1ull << g8)};

    static constexpr Bitboard EMPTY_Q[2] = {(1ull << d1) | (1ull << c1) | (1ull << b1),
                                            (1ull << d8) | (1ull << c8) | (1ull << b8)};

    static constexpr Square PASS_K[2][2] = {{f1, g1}, {f8, g8}};

    static constexpr Square PASS_Q[2][2] = {{d1, c1}, {d8, c8}};

    const CastleRights cr = pos.getMetadata().castleRights;
    const Bitboard empty = pos.getBoard().getEmptySquares();

    // prevent castling out of check
    if (PositionUtil::isCheck(pos, Side)) {
        return;
    }

    // --- Kingside ---
    if (hasCastleRights(cr, KINGSIDE[Side]) && (empty & EMPTY_K[Side]) == EMPTY_K[Side] &&
        !pos.isAttacked(PASS_K[Side][0], otherColor(Side)) &&
        !pos.isAttacked(PASS_K[Side][1], otherColor(Side))) {
        moveList.push_back(createCastlingMove<false, Side>());
    }

    // --- Queenside ---
    if (hasCastleRights(cr, QUEENSIDE[Side]) && (empty & EMPTY_Q[Side]) == EMPTY_Q[Side] &&
        !pos.isAttacked(PASS_Q[Side][0], otherColor(Side)) &&
        !pos.isAttacked(PASS_Q[Side][1], otherColor(Side))) {
        moveList.push_back(createCastlingMove<true, Side>());
    }
}

template<Color Side>
void generatePseudolegal(std::vector<Move> &result, Position &pos) {
    // update occupancy variables for quick access later
    friendlyPieces = pos.getBoard().getOccupancy(Side);
    enemyPieces = pos.getBoard().getOccupancy(otherColor(Side));
    occupiedSquares = friendlyPieces | enemyPieces;
    emptySquares = ~occupiedSquares;

    /* PAWNS */
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnPushes<Side>);
    appendMovesFromPiece<PAWN, Move::NORMAL>(result, pos, MoveComputers::computePawnCaptures<Side>);
    appendMovesFromPiece<PAWN, Move::EN_PASSANT>(result, pos,
                                                 MoveComputers::computePawnEnPassant<Side>);

    /* KNIGHTS */
    appendMovesFromPiece<KNIGHT, Move::NORMAL>(result, pos,
                                               MoveComputers::computeKnightMoves<Side>);

    /* BISHOPS, ROOKS, QUEENS */
    generateSlidingPieceMoves<BISHOP, Side>(result, pos);
    generateSlidingPieceMoves<ROOK, Side>(result, pos);
    generateSlidingPieceMoves<QUEEN, Side>(result, pos);

    /* KING */
    appendMovesFromPiece<KING, Move::NORMAL>(result, pos, MoveComputers::computeKingMoves<Side>);

    /* CASTLING */
    generateCastlingMoves<Side>(result, pos);
}

void generatePseudolegal(std::vector<Move> &result, Position &pos) {
    if (pos.getSideToMove() == WHITE) {
        generatePseudolegal<WHITE>(result, pos);
    } else {
        generatePseudolegal<BLACK>(result, pos);
    }
}

void generateLegal(std::vector<Move> &result, Position &pos) {
    result.clear();
    std::vector<Move> pseudolegal;
    generatePseudolegal(pseudolegal, pos);

    // only copy non-violating moves to legal vector
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
                 [&](Move m) { return isMoveLegal(pos, m); });
}

// used for quiescence search
void generatePseudolegalCaptures(std::vector<Move> &result, Position &pos) {
    result.clear();
    std::vector<Move> pseudolegal;
    generatePseudolegal(pseudolegal, pos);

    // only copy legal captures
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(result),
                 [&](Move m) { return isCaptureMove(pos, m); });
}

}  // namespace MoveGenerator
