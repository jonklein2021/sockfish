#include "Position.h"
#include "bit_tools.h"
#include "types.h"

#include <memory>

namespace MoveComputers {

Bitboard computePawnMoves(std::shared_ptr<Position> pos, Square sq) {
    const Color side = pos->getSideToMove();
    const Bitboard emptySquares = pos->board.getOccupancy(EMPTY_OCCUPANCY);
    if (side == WHITE) {
        const Bitboard singlePush = (1ull << (sq + NORTH)) & emptySquares;
        const Bitboard doublePush = (1ull << (sq + 2 * NORTH)) & rank4 & emptySquares;
        return singlePush | (doublePush & -singlePush);
    } else {
        const Bitboard singlePush = (1ull << (sq + SOUTH)) & emptySquares;
        const Bitboard doublePush = (1ull << (sq + 2 * SOUTH)) & rank5 & emptySquares;
        return singlePush | (doublePush & -singlePush);
    }
}

// ensure that there is a piece to capture
Bitboard computePawnAttacks(std::shared_ptr<Position> pos, Square sq) {
    const Color side = pos->getSideToMove();
    const Bitboard oppPieces = pos->board.getOccupancy(OccupancyType(otherColor(side)));
    const Bitboard epBB = 1ull << pos->md.enPassantSquare;
    if (side == WHITE) {
        // up right, up left
        return ((1ull << (sq + NORTH_EAST)) & not_file_a & (oppPieces | epBB)) |
               ((1ull << (sq + NORTH_WEST)) & not_file_h & (oppPieces | epBB));
    } else {
        // down right, down left
        return ((1ull << (sq + SOUTH_EAST)) & not_file_a & (oppPieces | epBB)) |
               ((1ull << (sq + SOUTH_WEST)) & not_file_h & (oppPieces | epBB));
    }
}

Bitboard computeKnightAttacks(std::shared_ptr<Position> _, Square sq) {
    const Bitboard sqBB = 1ull << sq;
    const Bitboard ddl = (sqBB & not_rank_12 & not_file_a) << 15;  // down 2, left 1
    const Bitboard ddr = (sqBB & not_rank_12 & not_file_h) << 17;  // down 2, right 1
    const Bitboard drr = (sqBB & not_rank_1 & not_file_gh) << 10;  // down 1, right 2
    const Bitboard dll = (sqBB & not_rank_1 & not_file_ab) << 6;   // down 1, left 2
    const Bitboard uur = (sqBB & not_rank_78 & not_file_h) >> 15;  // up 2, right 1
    const Bitboard uul = (sqBB & not_rank_78 & not_file_a) >> 17;  // up 2, left 1
    const Bitboard ull = (sqBB & not_rank_8 & not_file_ab) >> 10;  // up 1, left 2
    const Bitboard urr = (sqBB & not_rank_8 & not_file_gh) >> 6;   // up 1, right 2

    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

Bitboard computeBishopAttacks(std::shared_ptr<Position> pos, Square sq) {
    Bitboard attacks = 0;
    const auto board = pos->getBoard();

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up right
    for (int r = r0 + 1, c = c0 + 1; r <= 7 && c <= 7; r++, c++) {
        const Bitboard attackedSquare = (1ull << xyToSquare(c, r));
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, c = c0 - 1; r <= 7 && c >= 0; r++, c--) {
        const Bitboard attackedSquare = (1ull << xyToSquare(c, r));
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // down right
    for (int r = r0 - 1, c = c0 + 1; r >= 0 && c <= 7; r--, c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, c = c0 - 1; r >= 0 && c >= 0; r--, c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    return attacks;
}

Bitboard computeRookAttacks(std::shared_ptr<Position> pos, Square sq) {
    Bitboard attacks = 0;
    const Board board = pos->board;

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up
    for (int r = r0 + 1; r <= 7; r++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // down
    for (int r = r0 - 1; r >= 0; r--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // left
    for (int c = c0 - 1; c >= 0; c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // right
    for (int c = c0 + 1; c <= 7; c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    return attacks;
}

Bitboard computeQueenAttacks(std::shared_ptr<Position> pos, Square sq) {
    return computeBishopAttacks(pos, sq) | computeRookAttacks(pos, sq);  // heheh
}

Bitboard computeKingAttacks(std::shared_ptr<Position> _, Square sq) {
    const Bitboard sqBB = 1ull << sq;
    const Bitboard d = (sqBB & not_rank_1) << 8;                // down
    const Bitboard u = (sqBB & not_rank_8) >> 8;                // up
    const Bitboard l = (sqBB & not_file_a) >> 1;                // left
    const Bitboard r = (sqBB & not_file_h) << 1;                // right
    const Bitboard dl = (sqBB & not_rank_1 & not_file_a) << 7;  // down left
    const Bitboard dr = (sqBB & not_rank_1 & not_file_h) << 9;  // down right
    const Bitboard ul = (sqBB & not_rank_8 & not_file_a) >> 9;  // up left
    const Bitboard ur = (sqBB & not_rank_8 & not_file_h) >> 7;  // up right

    return d | u | l | r | dl | dr | ul | ur;
}

Bitboard computePieceAttacks(std::shared_ptr<Position> pos, Piece piece) {
    Bitboard attacks = 0;
    Bitboard pieceBitboard = pos->board.getPieces(piece);
    while (pieceBitboard) {
        const Bitboard sqBB = pieceBitboard & -pieceBitboard;  // isolate the LSB
        const Square sq = Square(indexOfLs1b(sqBB));

        // compute attacks for this piece
        switch (piece) {
            case WP:
            case BP: attacks |= computePawnAttacks(pos, sq); break;
            case WN:
            case BN: attacks |= computeKnightAttacks(pos, sq); break;
            case WB:
            case BB: attacks |= computeBishopAttacks(pos, sq); break;
            case WR:
            case BR: attacks |= computeRookAttacks(pos, sq); break;
            case WQ:
            case BQ: attacks |= computeQueenAttacks(pos, sq); break;
            case WK:
            case BK: attacks |= computeKingAttacks(pos, sq); break;
            default: return 0;
        }

        pieceBitboard ^= sqBB;  // remove the LSB from the bitboard
    }
    return attacks;
}

Bitboard computeAllSidesAttacks(std::shared_ptr<Position> pos) {
    Bitboard attacks = 0;
    const Color color = pos->getSideToMove();
    const int startIndex = (color == WHITE) ? 0 : 6;
    const int endIndex = (color == WHITE) ? 6 : 12;
    for (int p = startIndex; p < endIndex; p++) {
        attacks |= computePieceAttacks(pos, static_cast<Piece>(p));
    }
    return attacks;
}

}  // namespace MoveComputers
