#include "MoveComputers.h"

#include "src/bitboard/Magic.h"
#include "src/core/types.h"

namespace MoveComputers {

Bitboard computePawnPushes(const Position &pos, Square sq) {
    static constexpr int DIR[2] = {NORTH, SOUTH};

    static constexpr Bitboard TOGGLE[2] = {0x0, 0xFFFFFFFFFFFFFFFF};

    // double pawn pushes must land on these ranks
    static constexpr Bitboard DBL[2] = {RANK_MASKS[RANK_4], RANK_MASKS[RANK_5]};

    const Color side = pieceColor(pos.pieceAt(sq));
    const Bitboard emptySquares = pos.getBoard().getEmptySquares();

    Bitboard moves = 0ull;

    const Bitboard singlePushBB = (1ull << (sq + DIR[side])) & emptySquares;
    const Bitboard doublePushBB = (1ull << (sq + 2 * DIR[side])) & emptySquares;

    moves |= singlePushBB;
    moves |= (doublePushBB & DBL[side] & TOGGLE[!!singlePushBB]);

    return moves;
}

// used to compute attacked squares; does not guarantee a capture
Bitboard computePawnAttacks(const Position &pos, Square sq) {
    static constexpr int ATTACKS[2][2] = {
        {NORTH_EAST, NORTH_WEST},  // white
        {SOUTH_EAST, SOUTH_WEST}   // black
    };

    const Color side = pieceColor(pos.pieceAt(sq));
    Bitboard attacks = 0ull;

    const Bitboard eastCaptureBB = 1ull << (sq + ATTACKS[side][0]);
    const Bitboard westCaptureBB = 1ull << (sq + ATTACKS[side][1]);

    // only add to attacks if not out of bounds
    attacks |= (eastCaptureBB & notA);
    attacks |= (westCaptureBB & notH);

    return attacks;
}

Bitboard computePawnCaptures(const Position &pos, Square sq) {
    const Color side = pieceColor(pos.pieceAt(sq));
    const Bitboard oppPieces = pos.getBoard().getOccupancy(otherColor(side));
    return computePawnAttacks(pos, sq) & oppPieces;
}

Bitboard computePawnEnPassant(const Position &pos, Square sq) {
    const Square epSq = pos.getMetadata().enPassantSquare;
    if (epSq == NO_SQ) {
        return 0ull;
    }
    const Bitboard epSqBB = 1ull << epSq;
    return computePawnAttacks(pos, sq) & epSqBB;
}

Bitboard computeKnightMoves(const Position &pos, Square sq) {
    // this represents all possible moves from sq on an empty board
    const Bitboard knightMask = KNIGHT_MASKS[sq];

    // ensure the knight can land on each square
    const Color opponent = otherColor(pieceColor(pos.pieceAt(sq)));
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return knightMask & landingSqBB;
}

Bitboard computeKingMoves(const Position &pos, Square sq) {
    // this represents all possible moves from sq on an empty board
    const Bitboard kingMask = KING_MASKS[sq];

    // ensure king can land on square
    const Color opponent = otherColor(pieceColor(pos.pieceAt(sq)));
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return kingMask & landingSqBB;
}

Bitboard computeBishopMoves(const Position &pos, Square sq) {
    const Color side = pieceColor(pos.pieceAt(sq));
    const Bitboard blockers = pos.board.getOccupancies();
    const Bitboard ourPieces = pos.board.getOccupancy(side);
    const Bitboard moves = Magic::getBishopAttacks(sq, blockers);

    // prevent capturing own pieces
    return moves & ~ourPieces;
}

Bitboard computeRookMoves(const Position &pos, Square sq) {
    const Color side = pieceColor(pos.pieceAt(sq));
    Bitboard blockers = pos.board.getOccupancies();
    const Bitboard ourPieces = pos.board.getOccupancy(side);
    const Bitboard moves = Magic::getRookAttacks(sq, blockers);

    // prevent capturing own pieces
    return moves & ~ourPieces;
}

Bitboard computeQueenMoves(const Position &pos, Square sq) {
    const Color side = pieceColor(pos.pieceAt(sq));
    Bitboard blockers = pos.board.getOccupancies();
    const Bitboard ourPieces = pos.board.getOccupancy(side);
    const Bitboard moves = Magic::getQueenAttacks(sq, blockers);

    // prevent capturing own pieces
    return moves & ~ourPieces;
}

}  // namespace MoveComputers
