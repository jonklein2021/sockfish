#include "src/bitboard/bit_tools.h"
#include "src/core/types.h"

////////////////////
// Mask Computers //
////////////////////

constexpr Bitboard computeKnightMask(Square sq) {
    const Bitboard sqBB = 1ull << sq;
    const Bitboard ddl = (sqBB & NOT_RANK_1_NOR_2 & NOT_FILE_A) << 15;  // down 2, left 1
    const Bitboard ddr = (sqBB & NOT_RANK_1_NOR_2 & NOT_FILE_H) << 17;  // down 2, right 1
    const Bitboard drr = (sqBB & NOT_RANK_1 & NOT_FILE_GH) << 10;       // down 1, right 2
    const Bitboard dll = (sqBB & NOT_RANK_1 & NOT_FILE_AB) << 6;        // down 1, left 2
    const Bitboard uur = (sqBB & NOT_RANK_7_NOR_8 & NOT_FILE_H) >> 15;  // up 2, right 1
    const Bitboard uul = (sqBB & NOT_RANK_7_NOR_8 & NOT_FILE_A) >> 17;  // up 2, left 1
    const Bitboard ull = (sqBB & NOT_RANK_8 & NOT_FILE_AB) >> 10;       // up 1, left 2
    const Bitboard urr = (sqBB & NOT_RANK_8 & NOT_FILE_GH) >> 6;        // up 1, right 2

    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

constexpr Bitboard computeBishopMask(Square sq) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down right
    for (int r = r0 + 1, f = f0 + 1; r <= 6 && f <= 6; r++, f++) {
        allDstSqBB |= xyToBit(f, r);
    }

    // up left
    for (int r = r0 + 1, f = f0 - 1; r <= 6 && f >= 1; r++, f--) {
        allDstSqBB |= xyToBit(f, r);
    }

    // up right
    for (int r = r0 - 1, f = f0 + 1; r >= 1 && f <= 6; r--, f++) {
        allDstSqBB |= xyToBit(f, r);
    }

    // down left
    for (int r = r0 - 1, f = f0 - 1; r >= 1 && f >= 1; r--, f--) {
        allDstSqBB |= xyToBit(f, r);
    }

    return allDstSqBB;
}

constexpr Bitboard computeRookMask(Square sq) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down
    for (int r = r0 + 1; r <= 6; r++) {
        allDstSqBB |= xyToBit(f0, r);
    }

    // up
    for (int r = r0 - 1; r >= 1; r--) {
        allDstSqBB |= xyToBit(f0, r);
    }

    // left
    for (int f = f0 - 1; f >= 1; f--) {
        allDstSqBB |= xyToBit(f, r0);
    }

    // right
    for (int f = f0 + 1; f <= 6; f++) {
        allDstSqBB |= xyToBit(f, r0);
    }

    return allDstSqBB;
}

constexpr Bitboard computeQueenMask(Square sq) {
    return computeRookMask(sq) | computeBishopMask(sq);
}

constexpr Bitboard computeKingMask(Square sq) {
    const Bitboard sqBB = 1ull << sq;

    const Bitboard d = (sqBB & NOT_RANK_1) << 8;                // down
    const Bitboard u = (sqBB & NOT_RANK_8) >> 8;                // up
    const Bitboard l = (sqBB & NOT_FILE_A) >> 1;                // left
    const Bitboard r = (sqBB & NOT_FILE_H) << 1;                // right
    const Bitboard dl = (sqBB & NOT_RANK_1 & NOT_FILE_A) << 7;  // down left
    const Bitboard dr = (sqBB & NOT_RANK_1 & NOT_FILE_H) << 9;  // down right
    const Bitboard ul = (sqBB & NOT_RANK_8 & NOT_FILE_A) >> 9;  // up left
    const Bitboard ur = (sqBB & NOT_RANK_8 & NOT_FILE_H) >> 7;  // up right

    return d | u | l | r | dl | dr | ul | ur;
}

//////////////////////
// Mask Populators //
/////////////////////

template<typename MaskComputer>
constexpr std::array<Bitboard, NO_SQ> populateMaskArray(MaskComputer computer) {
    std::array<Bitboard, NO_SQ> masks{};
    for (Square sq : ALL_SQUARES) {
        masks[sq] = computer(sq);
    }
    return masks;
}

// these pre-computed masks represent all possible squares a piece can move to on an empty board
constexpr std::array<Bitboard, NO_SQ> KNIGHT_MASKS = populateMaskArray(computeKnightMask);
constexpr std::array<Bitboard, NO_SQ> BISHOP_MASKS = populateMaskArray(computeBishopMask);
constexpr std::array<Bitboard, NO_SQ> ROOK_MASKS = populateMaskArray(computeRookMask);
constexpr std::array<Bitboard, NO_SQ> QUEEN_MASKS = populateMaskArray(computeQueenMask);
constexpr std::array<Bitboard, NO_SQ> KING_MASKS = populateMaskArray(computeKingMask);
