#include "MoveGenerator.h"

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

inline bool isMoveLegal(Position &pos, Move &move) {
    // simulate move
    const Position::Metadata md = pos.makeMove(move);

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

template<Color Side>
inline void generatePawnMoves(std::vector<Move> &result, Position &pos) {
    constexpr Direction dir = Side == WHITE ? NORTH : SOUTH;
    constexpr Bitboard PROMOTING_RANK = Side == WHITE ? RANK_MASKS[RANK_8] : RANK_MASKS[RANK_1];

    // double pawn pushes must land on these ranks
    constexpr Bitboard dblEndRank = RANK_MASKS[Side == WHITE ? RANK_4 : RANK_5];

    const Bitboard bb = pos.getPieceBB(ptToPiece(PAWN, Side));
    forEachSquare(bb, [&](Square srcSq) {
        const Bitboard singlePushBB = (1ull << (srcSq + dir)) & emptySquares;
        const Bitboard doublePushBB = shift(singlePushBB, dir) & emptySquares;
        const Bitboard pseudoAttacksBB = PAWN_ATTACK_MASKS[Side][srcSq];

        Bitboard moves = 0ull;

        // add single pushes, double pushes, and standard captures
        moves |= singlePushBB;
        moves |= (doublePushBB & dblEndRank);
        moves |= (pseudoAttacksBB & enemyPieces);

        // separate promotion and quiet moves
        const Bitboard promotionsBB = moves & PROMOTING_RANK;
        const Bitboard quietMovesBB = moves & ~PROMOTING_RANK;

        appendMovesFromBitboard<Move::NORMAL>(result, quietMovesBB, srcSq);
        appendMovesFromBitboard<Move::PROMOTION>(result, promotionsBB, srcSq);

        // handle en passant separately
        const Square epSq = pos.getEpSquare();
        if (epSq != NO_SQ && getBit(pseudoAttacksBB, epSq)) {
            result.emplace_back(Move::create<Move::EN_PASSANT>(srcSq, epSq));
        }
    });
}

template<PieceType Pt, Color Side>
inline void generateSlidingPieceMoves(std::vector<Move> &result, Position &pos) {
    assert(Pt == BISHOP || Pt == ROOK || Pt == QUEEN);
    Bitboard bb = pos.getPieceBB(ptToPiece(Pt, Side));
    forEachSquare(bb, [&](Square srcSq) {
        Bitboard attacks =
            Magic::getSlidingPieceAttacks<Pt>(srcSq, occupiedSquares) & ~friendlyPieces;
        appendMovesFromBitboard<Move::NORMAL>(result, attacks, srcSq);
    });
}

// handles knight and king
template<PieceType Pt, Color Side>
inline void generateHoppingPieceMoves(std::vector<Move> &result, Position &pos) {
    assert(Pt == KING || Pt == KNIGHT);
    constexpr MoveMaskTable pseudoAttackTable = Pt == KING ? KING_MASKS : KNIGHT_MASKS;
    Bitboard bb = pos.getPieceBB(ptToPiece(Pt, Side));
    forEachSquare(bb, [&](Square srcSq) {
        const Bitboard moves = pseudoAttackTable[srcSq] & ~friendlyPieces;
        appendMovesFromBitboard<Move::NORMAL>(result, moves, srcSq);
    });
}

template<Color Side>
inline void generateCastlingMoves(std::vector<Move> &moveList, Position &pos) {
    constexpr Color enemy = otherColor(Side);

    constexpr CastleRights KINGSIDE = Side == WHITE ? WHITE_OO : BLACK_OO;
    constexpr CastleRights QUEENSIDE = Side == WHITE ? WHITE_OOO : BLACK_OOO;

    constexpr Bitboard EMPTY_K =
        Side == WHITE ? (1ull << f1) | (1ull << g1) : (1ull << f8) | (1ull << g8);

    constexpr Bitboard EMPTY_Q = Side == WHITE ? (1ull << d1) | (1ull << c1) | (1ull << b1)
                                               : (1ull << d8) | (1ull << c8) | (1ull << b8);

    constexpr std::array<Square, 2> PROTECTED_K =
        (Side == WHITE) ? std::array<Square, 2> {f1, g1} : std::array<Square, 2> {f8, g8};

    constexpr std::array<Square, 2> PROTECTED_Q =
        (Side == WHITE) ? std::array<Square, 2> {d1, c1} : std::array<Square, 2> {d8, c8};

    // prevent castling out of check
    if (PositionUtil::isCheck(pos, Side)) {
        return;
    }

    const CastleRights cr = pos.getCastleRights();

    // --- Kingside ---
    if (hasCastleRights(cr, KINGSIDE) && (emptySquares & EMPTY_K) == EMPTY_K &&
        !pos.isAttacked<enemy>(PROTECTED_K[0]) && !pos.isAttacked<enemy>(PROTECTED_K[1])) {
        moveList.push_back(createCastlingMove<false, Side>());
    }

    // --- Queenside ---
    if (hasCastleRights(cr, QUEENSIDE) && (emptySquares & EMPTY_Q) == EMPTY_Q &&
        !pos.isAttacked<enemy>(PROTECTED_Q[0]) && !pos.isAttacked<enemy>(PROTECTED_Q[1])) {
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
    generatePawnMoves<Side>(result, pos);

    /* KNIGHTS, KING */
    generateHoppingPieceMoves<KNIGHT, Side>(result, pos);
    generateHoppingPieceMoves<KING, Side>(result, pos);

    /* BISHOPS, ROOKS, QUEENS */
    generateSlidingPieceMoves<BISHOP, Side>(result, pos);
    generateSlidingPieceMoves<ROOK, Side>(result, pos);
    generateSlidingPieceMoves<QUEEN, Side>(result, pos);

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

// N.B: this is only really used by the UI to check user moves
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

template<Color Side>
void generatePtMoves(std::vector<Move> &result, Position &pos, PieceType pt) {
    switch (pt) {
        case PAWN: generatePawnMoves<Side>(result, pos); break;
        case KNIGHT: generateHoppingPieceMoves<KNIGHT, Side>(result, pos); break;
        case BISHOP: generateSlidingPieceMoves<BISHOP, Side>(result, pos); break;
        case ROOK: generateSlidingPieceMoves<ROOK, Side>(result, pos); break;
        case QUEEN: generateSlidingPieceMoves<QUEEN, Side>(result, pos); break;
        case KING: generateHoppingPieceMoves<KING, Side>(result, pos); break;
        case NO_PT: break;
    }
}

void generatePtMoves(std::vector<Move> &result, Position &pos, PieceType pt) {
    if (pos.getSideToMove() == WHITE) {
        generatePtMoves<WHITE>(result, pos, pt);
    } else {
        generatePtMoves<BLACK>(result, pos, pt);
    }
}

}  // namespace MoveGenerator
