#include "MoveComputers.h"

#include "Magic.h"
#include "types.h"

namespace MoveComputers {

Bitboard computePawnPushes(const Position &pos, Square sq) {
    static constexpr Direction DIR[2] = {NORTH, SOUTH};

    static constexpr Bitboard TOGGLE[2] = {0x0, 0xFFFFFFFFFFFFFFFF};

    // double pawn pushes must land on these ranks
    static constexpr Bitboard DBL[2] = {RANK_4, RANK_5};

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
    attacks |= (eastCaptureBB & NOT_FILE_A);
    attacks |= (westCaptureBB & NOT_FILE_H);

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
    const Bitboard sqBB = 1ull << sq;

    const Bitboard ddl = (sqBB & NOT_RANK_1_NOR_2 & NOT_FILE_A) << 15;  // down 2, left 1
    const Bitboard ddr = (sqBB & NOT_RANK_1_NOR_2 & NOT_FILE_H) << 17;  // down 2, right 1
    const Bitboard drr = (sqBB & NOT_RANK_1 & NOT_FILE_GH) << 10;       // down 1, right 2
    const Bitboard dll = (sqBB & NOT_RANK_1 & NOT_FILE_AB) << 6;        // down 1, left 2
    const Bitboard uur = (sqBB & NOT_RANK_7_NOR_8 & NOT_FILE_H) >> 15;  // up 2, right 1
    const Bitboard uul = (sqBB & NOT_RANK_7_NOR_8 & NOT_FILE_A) >> 17;  // up 2, left 1
    const Bitboard ull = (sqBB & NOT_RANK_8 & NOT_FILE_AB) >> 10;       // up 1, left 2
    const Bitboard urr = (sqBB & NOT_RANK_8 & NOT_FILE_GH) >> 6;        // up 1, right 2

    const Color opponent = otherColor(pieceColor(pos.pieceAt(sq)));
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return (ddl | ddr | drr | dll | uur | uul | ull | urr) & landingSqBB;
}

Bitboard computeKingMoves(const Position &pos, Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard d = (sqBB & NOT_RANK_1) << 8;                // down
    const Bitboard u = (sqBB & NOT_RANK_8) >> 8;                // up
    const Bitboard l = (sqBB & NOT_FILE_A) >> 1;                // left
    const Bitboard r = (sqBB & NOT_FILE_H) << 1;                // right
    const Bitboard dl = (sqBB & NOT_RANK_1 & NOT_FILE_A) << 7;  // down left
    const Bitboard dr = (sqBB & NOT_RANK_1 & NOT_FILE_H) << 9;  // down right
    const Bitboard ul = (sqBB & NOT_RANK_8 & NOT_FILE_A) >> 9;  // up left
    const Bitboard ur = (sqBB & NOT_RANK_8 & NOT_FILE_H) >> 7;  // up right

    const Color opponent = otherColor(pieceColor(pos.pieceAt(sq)));
    const Bitboard landingSqBB = pos.board.getEmptySquares() | pos.board.getOccupancy(opponent);

    return (d | u | l | r | dl | dr | ul | ur) & landingSqBB;
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
    const Bitboard blockers = pos.board.getOccupancies();
    const Bitboard ourPieces = pos.board.getOccupancy(side);
    const Bitboard moves = Magic::getRookAttacks(sq, blockers);

    // prevent capturing own pieces
    return moves & ~ourPieces;
}

Bitboard computeQueenMoves(const Position &pos, Square sq) {
    return computeBishopMoves(pos, sq) | computeRookMoves(pos, sq);  // DRY
}

}  // namespace MoveComputers
