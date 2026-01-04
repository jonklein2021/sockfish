#include "MoveComputers.h"

namespace MoveComputers {

Bitboard computeKnightMoves(std::shared_ptr<Position> pos, Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard ddl = (sqBB & not_rank_12 & not_file_a) << 15;  // down 2, left 1
    const Bitboard ddr = (sqBB & not_rank_12 & not_file_h) << 17;  // down 2, right 1
    const Bitboard drr = (sqBB & not_rank_1 & not_file_gh) << 10;  // down 1, right 2
    const Bitboard dll = (sqBB & not_rank_1 & not_file_ab) << 6;   // down 1, left 2
    const Bitboard uur = (sqBB & not_rank_78 & not_file_h) >> 15;  // up 2, right 1
    const Bitboard uul = (sqBB & not_rank_78 & not_file_a) >> 17;  // up 2, left 1
    const Bitboard ull = (sqBB & not_rank_8 & not_file_ab) >> 10;  // up 1, left 2
    const Bitboard urr = (sqBB & not_rank_8 & not_file_gh) >> 6;   // up 1, right 2

    const Color opponent = otherColor(pos->getSideToMove());
    const Bitboard landingSqBB = pos->board.getEmptySquares() | pos->board.getOccupancy(opponent);

    return (ddl | ddr | drr | dll | uur | uul | ull | urr) & landingSqBB;
}

Bitboard computeBishopMoves(std::shared_ptr<Position> pos, Square sq) {
    Bitboard attacks = 0;
    const Color side = pos->getSideToMove();
    const Color opponentSide = otherColor(side);
    const Board board = pos->getBoard();

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up right
    for (int r = r0 + 1, c = c0 + 1; r <= 7 && c <= 7; r++, c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, c = c0 - 1; r <= 7 && c >= 0; r++, c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    // down right
    for (int r = r0 - 1, c = c0 + 1; r >= 0 && c <= 7; r--, c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, c = c0 - 1; r >= 0 && c >= 0; r--, c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    return attacks;
}

Bitboard computeRookMoves(std::shared_ptr<Position> pos, Square sq) {
    Bitboard attacks = 0;
    const Color side = pos->getSideToMove();
    const Color opponentSide = otherColor(side);
    const Board board = pos->getBoard();

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up
    for (int r = r0 + 1; r <= 7; r++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c0, r);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    // down
    for (int r = r0 - 1; r >= 0; r--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c0, r);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    // left
    for (int c = c0 - 1; c >= 0; c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r0);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    // right
    for (int c = c0 + 1; c <= 7; c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r0);
        if (attackedSquare & board.getOccupancy(side)) {
            break;
        }
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(opponentSide)) {
            break;
        }
    }

    return attacks;
}

Bitboard computeQueenMoves(std::shared_ptr<Position> pos, Square sq) {
    return computeBishopMoves(pos, sq) | computeRookMoves(pos, sq);  // heheh
}

Bitboard computeKingMoves(std::shared_ptr<Position> pos, Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard d = (sqBB & not_rank_1) << 8;                // down
    const Bitboard u = (sqBB & not_rank_8) >> 8;                // up
    const Bitboard l = (sqBB & not_file_a) >> 1;                // left
    const Bitboard r = (sqBB & not_file_h) << 1;                // right
    const Bitboard dl = (sqBB & not_rank_1 & not_file_a) << 7;  // down left
    const Bitboard dr = (sqBB & not_rank_1 & not_file_h) << 9;  // down right
    const Bitboard ul = (sqBB & not_rank_8 & not_file_a) >> 9;  // up left
    const Bitboard ur = (sqBB & not_rank_8 & not_file_h) >> 7;  // up right

    const Color opponent = otherColor(pos->getSideToMove());
    const Bitboard landingSqBB = pos->board.getEmptySquares() | pos->board.getOccupancy(opponent);

    return (d | u | l | r | dl | dr | ul | ur) & landingSqBB;
}

}  // namespace MoveComputers
