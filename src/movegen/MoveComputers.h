#include "src/bitboard/Magic.h"
#include "src/core/Position.h"
#include "src/core/types.h"

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
    constexpr Direction dir = side == WHITE ? NORTH : SOUTH;

    Bitboard attacks = 0ull;

    const Bitboard eastCaptureBB = 1ull << (sq + Direction(dir + EAST));
    const Bitboard westCaptureBB = 1ull << (sq + Direction(dir + WEST));

    // only add to attacks if not out of bounds
    attacks |= (eastCaptureBB & notA);
    attacks |= (westCaptureBB & notH);

    return attacks;
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
    const Color opponent = otherColor(pieceColor(pos.pieceAt(sq)));
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return knightMask & landingSqBB;
}

template<Color side>
inline Bitboard computeKingMoves(const Position &pos, Square sq) {
    // this represents all possible moves from sq on an empty board
    const Bitboard moves = KING_MASKS[sq];

    // ensure king can land on square
    const Color opponent = otherColor(pieceColor(pos.pieceAt(sq)));
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return moves & landingSqBB;
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
