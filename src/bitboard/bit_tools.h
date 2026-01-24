#pragma once

#include "src/core/types.h"

/**
 * Returns the index of the least significant bit of a bitboard
 */
#define getLsbIndex(bb) __builtin_ctzll(bb)

/**
 * Returns the number of bits (population count) of a Bitboard
 */
#define getBitCount(bb) __builtin_popcountll(bb)

/**
 * Returns the bit at the given index
 *
 * @param bitboard the relevant bitboard
 * @param sq the square (index) of that bitboard to check
 * @return the bit at the given index
 */
inline static constexpr Bitboard getBit(Bitboard bb, Square sq) {
    return bb & (1ull << sq);
}

/**
 * Sets the bit at the given index
 *
 * @param bb the relevant bitboard
 * @param sq the square (index) of that bitboard to set
 */
inline static constexpr void setBit(Bitboard &bb, Square sq) {
    bb |= (1ull << sq);
}

/**
 * Unsets the bit at the given index
 *
 * @param bb the relevant bitboard
 * @param sq the square (index) of that bitboard to set
 */
inline static constexpr void unsetBit(Bitboard &bb, Square sq) {
    bb &= ~(1ull << sq);
}

/**
 * Converts a pair of coordinates to a bit
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @return the bit corresponding to the coordinates
 */
inline static constexpr Bitboard xyToBit(int x, int y) {
    return 1ull << xyToSquare(x, y);
}

template<typename Fn>
constexpr void forEachSquare(Bitboard bb, Fn fn) {
    while (bb) {
        Square sq = Square(getLsbIndex(bb));
        fn(sq);
        bb &= bb - 1;
    }
}
