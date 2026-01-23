#include "src/bitboard/Magic.h"
#include "src/core/Position.h"
#include "src/core/types.h"

#include <cassert>

namespace MoveComputers {

using MoveComputerFunc = Bitboard (*)(const Position &, Square);

template<Color side>
inline Bitboard computePawnPushes(const Position &pos, Square sq) {
    constexpr Direction dir = side == WHITE ? NORTH : SOUTH;

    // double pawn pushes must land on these ranks
    constexpr Bitboard dblEndRank = RANK_MASKS[RANK_4 - side];

    const Bitboard emptySquares = pos.getBoard().getEmptySquares();
    const Bitboard toggle = ALL_SQUARES_BB;

    Bitboard moves = 0ull;

    const Bitboard singlePushBB = (1ull << (sq + dir)) & emptySquares;
    const Bitboard doublePushBB = (1ull << (sq + 2 * dir)) & emptySquares;

    moves |= singlePushBB;
    moves |= (doublePushBB & dblEndRank & toggle * !!singlePushBB);

    return moves;
}

// used to compute attacked squares; does not guarantee a capture
template<Color side>
inline Bitboard computePawnAttacks(const Position &pos, Square sq) {
    assert(&pos);  // stupid way to avoid unused var warning
    return PAWN_ATTACK_MASKS[side][sq];
}

template<Color side>
inline Bitboard computePawnCaptures(const Position &pos, Square sq) {
    const Bitboard oppPieces = pos.getBoard().getOccupancy(otherColor(side));
    return computePawnAttacks<side>(pos, sq) & oppPieces;
}

template<Color side>
inline Bitboard computePawnEnPassant(const Position &pos, Square sq) {
    const Square epSq = pos.getMetadata().enPassantSquare;
    if (epSq == NO_SQ) {
        return 0ull;
    }
    const Bitboard epSqBB = 1ull << epSq;
    return computePawnAttacks<side>(pos, sq) & epSqBB;
}

template<Color side>
inline Bitboard computeKnightMoves(const Position &pos, Square sq) {
    // this represents all possible moves from sq on an empty board
    const Bitboard knightMask = KNIGHT_MASKS[sq];

    // ensure the knight can land on each square
    const Color opponent = otherColor(side);
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return knightMask & landingSqBB;
}

template<Color side>
inline Bitboard computeKingMoves(const Position &pos, Square sq) {
    // this represents all possible moves from sq on an empty board
    const Bitboard kingMask = KING_MASKS[sq];

    // ensure king can land on square
    const Color opponent = otherColor(side);
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return kingMask & landingSqBB;
}

template<Color side>
inline Bitboard computeBishopMoves(const Position &pos, Square sq) {
    const Bitboard blockers = pos.board.getOccupancies();
    const Bitboard ourPieces = pos.board.getOccupancy(side);
    const Bitboard moves = Magic::getBishopAttacks(sq, blockers);

    // prevent capturing own pieces
    return moves & ~ourPieces;
}

template<Color side>
inline Bitboard computeRookMoves(const Position &pos, Square sq) {
    const Bitboard blockers = pos.board.getOccupancies();
    const Bitboard ourPieces = pos.board.getOccupancy(side);
    const Bitboard moves = Magic::getRookAttacks(sq, blockers);

    // prevent capturing own pieces
    return moves & ~ourPieces;
}

template<Color side>
inline Bitboard computeQueenMoves(const Position &pos, Square sq) {
    const Bitboard blockers = pos.board.getOccupancies();
    const Bitboard ourPieces = pos.board.getOccupancy(side);
    const Bitboard moves = Magic::getQueenAttacks(sq, blockers);

    // prevent capturing own pieces
    return moves & ~ourPieces;
}

inline constexpr std::array<MoveComputerFunc, 12> moveAttackComputers = {
    MoveComputers::computePawnAttacks<WHITE>, MoveComputers::computeKnightMoves<WHITE>,
    MoveComputers::computeBishopMoves<WHITE>, MoveComputers::computeRookMoves<WHITE>,
    MoveComputers::computeQueenMoves<WHITE>,  MoveComputers::computeKingMoves<WHITE>,
    MoveComputers::computePawnAttacks<BLACK>, MoveComputers::computeKnightMoves<BLACK>,
    MoveComputers::computeBishopMoves<BLACK>, MoveComputers::computeRookMoves<BLACK>,
    MoveComputers::computeQueenMoves<BLACK>,  MoveComputers::computeKingMoves<BLACK>};
}  // namespace MoveComputers
