#include "Position.h"
#include "bit_tools.h"
#include "types.h"

#include <memory>

namespace MoveComputers {

Bitboard computePawnMoves(std::shared_ptr<Position> pos, Square sq) {
    static constexpr Direction DIR[2] = {NORTH, SOUTH};
    static constexpr Bitboard DBL[2] = {rank4, rank5};

    const Color side = pos->getSideToMove();
    const Bitboard emptySquares = pos->board.getOccupancy(EMPTY_OCCUPANCY);

    // destination square must be empty
    const Bitboard singlePush = (1ull << (sq + DIR[side])) & emptySquares;

    // double pawn push requires pawn to land on the 4th rank
    // AND 2 empty squares in front of it
    const Bitboard doublePush = (1ull << (sq + 2 * DIR[side])) & emptySquares & DBL[side];

    return singlePush | (doublePush & -!!singlePush);
}

// ensure that there is a piece to capture
Bitboard computePawnAttacks(std::shared_ptr<Position> pos, Square sq) {
    static constexpr int ATTACKS[2][2] = {
        {NORTH_EAST, NORTH_WEST},  // white
        {SOUTH_EAST, SOUTH_WEST}   // black
    };

    static constexpr Bitboard FILE_MASKS[2] = {
        not_file_a,  // east moves
        not_file_h   // west moves
    };

    const Color side = pos->getSideToMove();
    const Bitboard oppPieces = pos->board.getOccupancy(OccupancyType(otherColor(side)));

    const Bitboard epSqBB = (1ull << pos->md.enPassantSquare);
    const Bitboard targets = oppPieces | epSqBB;

    Bitboard attacks = 0;

    attacks |= (1ull << (sq + ATTACKS[side][0])) & FILE_MASKS[0] & targets;
    attacks |= (1ull << (sq + ATTACKS[side][1])) & FILE_MASKS[1] & targets;

    return attacks;
}

Bitboard computeKnightAttacks(std::shared_ptr<Position> pos, Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard ddl = (sqBB & not_rank_12 & not_file_a) << 15;  // down 2, left 1
    const Bitboard ddr = (sqBB & not_rank_12 & not_file_h) << 17;  // down 2, right 1
    const Bitboard drr = (sqBB & not_rank_1 & not_file_gh) << 10;  // down 1, right 2
    const Bitboard dll = (sqBB & not_rank_1 & not_file_ab) << 6;   // down 1, left 2
    const Bitboard uur = (sqBB & not_rank_78 & not_file_h) >> 15;  // up 2, right 1
    const Bitboard uul = (sqBB & not_rank_78 & not_file_a) >> 17;  // up 2, left 1
    const Bitboard ull = (sqBB & not_rank_8 & not_file_ab) >> 10;  // up 1, left 2
    const Bitboard urr = (sqBB & not_rank_8 & not_file_gh) >> 6;   // up 1, right 2

    const OccupancyType enemyOcc = OccupancyType(otherColor(pos->getSideToMove()));
    const Bitboard landingSqBB =
        pos->board.getOccupancy(EMPTY_OCCUPANCY) | pos->board.getOccupancy(enemyOcc);

    return (ddl | ddr | drr | dll | uur | uul | ull | urr) & landingSqBB;
}

Bitboard computeBishopAttacks(std::shared_ptr<Position> pos, Square sq) {
    Bitboard attacks = 0;
    const Color side = pos->getSideToMove();
    const OccupancyType friendlyOcc = OccupancyType(side);
    const OccupancyType opponentOcc = OccupancyType(otherColor(side));
    const Board board = pos->getBoard();

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up right
    for (int r = r0 + 1, c = c0 + 1; r <= 7 && c <= 7; r++, c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, c = c0 - 1; r <= 7 && c >= 0; r++, c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    // down right
    for (int r = r0 - 1, c = c0 + 1; r >= 0 && c <= 7; r--, c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, c = c0 - 1; r >= 0 && c >= 0; r--, c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    return attacks;
}

Bitboard computeRookAttacks(std::shared_ptr<Position> pos, Square sq) {
    Bitboard attacks = 0;
    const Color side = pos->getSideToMove();
    const OccupancyType friendlyOcc = OccupancyType(side);
    const OccupancyType opponentOcc = OccupancyType(otherColor(side));
    const Board board = pos->getBoard();

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up
    for (int r = r0 + 1; r <= 7; r++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c0, r);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    // down
    for (int r = r0 - 1; r >= 0; r--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c0, r);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    // left
    for (int c = c0 - 1; c >= 0; c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r0);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    // right
    for (int c = c0 + 1; c <= 7; c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r0);
        if (attackedSquare & board.getOccupancy(friendlyOcc)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentOcc)) {
            break;
        }
    }

    return attacks;
}

Bitboard computeQueenAttacks(std::shared_ptr<Position> pos, Square sq) {
    return computeBishopAttacks(pos, sq) | computeRookAttacks(pos, sq);  // heheh
}

Bitboard computeKingAttacks(std::shared_ptr<Position> pos, Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard d = (sqBB & not_rank_1) << 8;                // down
    const Bitboard u = (sqBB & not_rank_8) >> 8;                // up
    const Bitboard l = (sqBB & not_file_a) >> 1;                // left
    const Bitboard r = (sqBB & not_file_h) << 1;                // right
    const Bitboard dl = (sqBB & not_rank_1 & not_file_a) << 7;  // down left
    const Bitboard dr = (sqBB & not_rank_1 & not_file_h) << 9;  // down right
    const Bitboard ul = (sqBB & not_rank_8 & not_file_a) >> 9;  // up left
    const Bitboard ur = (sqBB & not_rank_8 & not_file_h) >> 7;  // up right

    const OccupancyType enemyOcc = OccupancyType(otherColor(pos->getSideToMove()));
    const Bitboard landingSqBB =
        pos->board.getOccupancy(EMPTY_OCCUPANCY) | pos->board.getOccupancy(enemyOcc);

    return (d | u | l | r | dl | dr | ul | ur) & landingSqBB;
}

Bitboard computePieceAttacks(std::shared_ptr<Position> pos, Piece piece) {
    Bitboard attacks = 0;
    Bitboard pieceBitboard = pos->getPieceBB(piece);
    while (pieceBitboard) {
        const Bitboard sqBB = pieceBitboard & -pieceBitboard;  // isolate the LSB
        const Square sq = Square(getLsbIndex(sqBB));

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
