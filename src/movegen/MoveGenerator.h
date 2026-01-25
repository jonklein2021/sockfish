#pragma once

#include "src/bitboard/bit_tools.h"
#include "src/core/Move.h"
#include "src/core/Position.h"
#include "src/core/PositionUtil.h"

#include <vector>

namespace MoveGenerator {

static constexpr Bitboard PROMOTING_RANKS[2] = {RANK_MASKS[RANK_8], RANK_MASKS[RANK_1]};

inline bool isMoveLegal(Position &pos, Move &move) {
    // simulate move
    Position::Metadata md = pos.makeMove(move);

    // test if king is in check after move
    bool isLegal = pos.isLegal();

    // unmake move to preserve original state
    pos.unmakeMove(move, md);

    // return true iff the move hasn't put its own king in check
    return isLegal;
}

inline bool isCaptureMove(Position &pos, Move &move) {
    return pos.pieceAt(move.getToSquare()) != NO_PIECE || move.isEnPassant();
}

template<Move::Type MoveType>
void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq) {
    forEachSquare(moves, [&](Square destSq) {
        moveList.emplace_back(Move::create<MoveType>(srcSq, destSq));
    });
}

template<>
inline void appendMovesFromBitboard<Move::PROMOTION>(std::vector<Move> &moveList,
                                                     Bitboard moves,
                                                     Square srcSq) {
    forEachSquare(moves, [&](Square destSq) {
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, KNIGHT));
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, BISHOP));
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, ROOK));
        moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, destSq, QUEEN));
    });
}

template<PieceType Pt, Move::Type MoveType, typename MoveComputer>
void appendMovesFromPiece(std::vector<Move> &moveList, Position &pos, MoveComputer moveComputer) {
    const Color toMove = pos.getSideToMove();
    const Piece piece = ptToPiece(Pt, toMove);
    Bitboard bb = pos.getPieceBB(piece);  // N.B: this needs to be a copy

    if constexpr (Pt == PAWN) {
        while (bb) {
            const Square srcSq = Square(getLsbIndex(bb));

            const Bitboard computedDstBB = moveComputer(pos, srcSq);

            // for pawns, separate destination squares into promotion and normal moves
            const Bitboard promotionsBB = computedDstBB & PROMOTING_RANKS[toMove];
            const Bitboard nonPromotionsBB = computedDstBB & ~PROMOTING_RANKS[toMove];

            // create and append moves to the moveList
            appendMovesFromBitboard<Move::PROMOTION>(moveList, promotionsBB, srcSq);
            appendMovesFromBitboard<MoveType>(moveList, nonPromotionsBB, srcSq);

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
            appendMovesFromBitboard<MoveType>(moveList, computedDstBB, srcSq);

            // pop LSB
            bb &= bb - 1;
        }
    }
}

template<bool IsQueenside, Color Side>
Move inline createCastlingMove() {
    constexpr Square KING_FROM = Side == WHITE ? e1 : e8;
    constexpr Square KING_TO = Side == WHITE ? (IsQueenside ? c1 : g1) : (IsQueenside ? c8 : g8);

    return Move::create<Move::CASTLING>(KING_FROM, KING_TO);
}

template<PieceType Pt, Color Side>
void generateSlidingPieceMoves(std::vector<Move> &result, Position &pos);

void generateLegal(std::vector<Move> &result, Position &pos);

template<Color Side>
void generatePseudolegal(std::vector<Move> &result, Position &pos);

void generatePseudolegal(std::vector<Move> &result, Position &pos);
void generatePseudolegalCaptures(std::vector<Move> &result, Position &pos);

}  // namespace MoveGenerator
