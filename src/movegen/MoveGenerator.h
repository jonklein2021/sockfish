#pragma once

#include "src/core/Move.h"
#include "src/core/Position.h"
#include "src/core/PositionUtil.h"

#include <vector>

namespace MoveGenerator {

static constexpr Bitboard PROMOTING_RANKS[2] = {RANK_MASKS[RANK_8], RANK_MASKS[RANK_1]};

inline bool isMoveLegal(Position &pos, Move &move) {
    Color moveMaker = pos.getSideToMove();

    // simulate move on copied state
    Position::Metadata md = pos.makeMove(move);

    // test if king is in check after move
    bool isOurKingInCheck = PositionUtil::isCheck(pos, moveMaker);

    // unmake move to preserve original state
    pos.unmakeMove(move, md);

    // return true iff the move hasn't put its own king in check
    return !isOurKingInCheck;
}

inline bool isCaptureMove(Position &pos, Move &move) {
    return pos.pieceAt(move.getToSquare()) != NO_PIECE || move.isEnPassant();
}

template<Move::Type moveType>
void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq) {
    while (moves) {
        const Square destSq = Square(getLsbIndex(moves));
        moveList.emplace_back(Move::create<moveType>(srcSq, destSq));
        moves &= moves - 1;
    }
}

template<>
inline void appendMovesFromBitboard<Move::PROMOTION>(std::vector<Move> &moveList,
                                                     Bitboard moves,
                                                     Square srcSq) {
    while (moves) {
        const Square destSq = Square(getLsbIndex(moves));
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, KNIGHT));
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, BISHOP));
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, ROOK));
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, QUEEN));
        moves &= moves - 1;
    }
}

template<PieceType pt, Move::Type moveType, typename MoveComputer>
void appendMovesFromPiece(std::vector<Move> &moveList, Position &pos, MoveComputer moveComputer) {
    const Color toMove = pos.getSideToMove();
    const Piece piece = ptToPiece(pt, toMove);
    Bitboard bb = pos.getPieceBB(piece);  // N.B: this needs to be a copy

    if constexpr (pt == PAWN) {
        while (bb) {
            const Square srcSq = Square(getLsbIndex(bb));

            const Bitboard computedDstBB = moveComputer(pos, srcSq);

            // for pawns, separate destination squares into promotion and normal moves
            const Bitboard promotionsBB = computedDstBB & PROMOTING_RANKS[toMove];
            const Bitboard nonPromotionsBB = computedDstBB & ~PROMOTING_RANKS[toMove];

            // create and append moves to the moveList
            appendMovesFromBitboard<Move::PROMOTION>(moveList, promotionsBB, srcSq);
            appendMovesFromBitboard<moveType>(moveList, nonPromotionsBB, srcSq);

            // pop LSB
            bb &= bb - 1;
        }
    } else {
        while (bb) {
            const Square srcSq = Square(getLsbIndex(bb));

            // compute a bitboard of all destination squares that this piece can go to,
            // according to the specified moveComputer
            const Bitboard computedDstBB = moveComputer(pos, srcSq);

            // create and append moves to the moveList
            appendMovesFromBitboard<moveType>(moveList, computedDstBB, srcSq);

            // pop LSB
            bb &= bb - 1;
        }
    }
}

Move inline createCastlingMove(bool isQueenside, Color side) {
    // {white, black}
    static constexpr Square KING_FROM[2] = {e1, e8};

    // {kingside: {white, black}, queenside: {white, black}}
    static constexpr Square KING_TO[2][2] = {{g1, g8}, {c1, c8}};

    return Move::create<Move::CASTLING>(KING_FROM[side], KING_TO[isQueenside][side]);
}

// consider creating a MoveComputer for this
inline void appendCastlingMoves(std::vector<Move> &moveList, Position &pos) {
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

    const Color side = pos.getSideToMove();
    const CastleRights cr = pos.getMetadata().castleRights;
    const Bitboard empty = pos.getBoard().getEmptySquares();

    // prevent castling out of check
    if (PositionUtil::isCheck(pos, side)) {
        return;
    }

    // --- Kingside ---
    if (hasCastleRights(cr, KINGSIDE[side]) && (empty & EMPTY_K[side]) == EMPTY_K[side] &&
        !pos.isAttacked(PASS_K[side][0], otherColor(side)) &&
        !pos.isAttacked(PASS_K[side][1], otherColor(side))) {
        moveList.push_back(createCastlingMove(false, side));
    }

    // --- Queenside ---
    if (hasCastleRights(cr, QUEENSIDE[side]) && (empty & EMPTY_Q[side]) == EMPTY_Q[side] &&
        !pos.isAttacked(PASS_Q[side][0], otherColor(side)) &&
        !pos.isAttacked(PASS_Q[side][1], otherColor(side))) {
        moveList.push_back(createCastlingMove(true, side));
    }
}

void generateLegal(std::vector<Move> &result, Position &pos);
void generatePseudolegal(std::vector<Move> &result, Position &pos);
void generatePseudolegalCaptures(std::vector<Move> &result, Position &pos);

}  // namespace MoveGenerator
