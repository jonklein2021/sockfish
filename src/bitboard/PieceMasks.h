#include "src/bitboard/bit_tools.h"
#include "src/core/types.h"

using MoveMaskTable = std::array<Bitboard, NO_SQ>;

/////////////////////////
// Move Mask Computers //
/////////////////////////

constexpr Bitboard computeKnightMask(Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard ddl = (sqBB & not12 & notA) << 15;  // down 2, left 1
    const Bitboard ddr = (sqBB & not12 & notH) << 17;  // down 2, right 1
    const Bitboard drr = (sqBB & not1 & notGH) << 10;  // down 1, right 2
    const Bitboard dll = (sqBB & not1 & notAB) << 6;   // down 1, left 2
    const Bitboard uur = (sqBB & not78 & notH) >> 15;  // up 2, right 1
    const Bitboard uul = (sqBB & not78 & notA) >> 17;  // up 2, left 1
    const Bitboard ull = (sqBB & not8 & notAB) >> 10;  // up 1, left 2
    const Bitboard urr = (sqBB & not8 & notGH) >> 6;   // up 1, right 2

    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

constexpr Bitboard computeRookMask(Square sq) {
    Bitboard mask = (RANK_MASKS[rankOf(sq)] & ~(FILE_MASKS[FILE_A] | FILE_MASKS[FILE_H])) |
                    (FILE_MASKS[fileOf(sq)] & ~(RANK_MASKS[RANK_1] | RANK_MASKS[RANK_8]));

    // unset source bit
    unsetBit(mask, sq);

    return mask;
}

constexpr Bitboard computeBishopMask(Square sq) {
    Bitboard allDstSqBB = 0ull;

    // initialize with all diagonal moves from this square
    allDstSqBB |= SLASH_DIAGONAL_MASKS[getSlashDiagonalIndex(sq)];
    allDstSqBB |= BACKSLASH_DIAGONAL_MASKS[getBackslashDiagonalIndex(sq)];

    // unset source bit
    unsetBit(allDstSqBB, sq);

    // unset outer bits because occupancies they're not needed for this step
    return allDstSqBB & not1 & not8 & notA & notH;
}

constexpr Bitboard computeQueenMask(Square sq) {
    return computeRookMask(sq) | computeBishopMask(sq);
}

constexpr Bitboard computeKingMask(Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard d = (sqBB & not1) << 8;          // down
    const Bitboard u = (sqBB & not8) >> 8;          // up
    const Bitboard l = (sqBB & notA) >> 1;          // left
    const Bitboard r = (sqBB & notH) << 1;          // right
    const Bitboard dl = (sqBB & not1 & notA) << 7;  // down left
    const Bitboard dr = (sqBB & not1 & notH) << 9;  // down right
    const Bitboard ul = (sqBB & not8 & notA) >> 9;  // up left
    const Bitboard ur = (sqBB & not8 & notH) >> 7;  // up right

    return d | u | l | r | dl | dr | ul | ur;
}

///////////////////////////
// Mask Table Populators //
//////////////////////////

template<typename Fn>
constexpr MoveMaskTable fillMoveMaskTable(Fn fn) {
    MoveMaskTable table{};
    for (Square sq : ALL_SQUARES) {
        table[sq] = fn(sq);
    }
    return table;
}

// these pre-computed masks represent all possible squares a piece can move to on an empty board
constexpr MoveMaskTable KNIGHT_MASKS = fillMoveMaskTable(computeKnightMask);
constexpr MoveMaskTable BISHOP_MASKS = fillMoveMaskTable(computeBishopMask);
constexpr MoveMaskTable ROOK_MASKS = fillMoveMaskTable(computeRookMask);
constexpr MoveMaskTable QUEEN_MASKS = fillMoveMaskTable(computeQueenMask);
constexpr MoveMaskTable KING_MASKS = fillMoveMaskTable(computeKingMask);
