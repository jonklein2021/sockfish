#pragma once

#include "src/core/types.h"

// minimal swap implementation so I don't need to include the entire utility header
template<typename T>
inline static constexpr void my_swap(T &x, T &y) {
    T temp = x;
    x = y;
    y = temp;
}

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
    return 1ull << (y * 8 + x);
}

/**
 * Pops (removes and returns) the bit at the given index
 *
 * @param bb the relevant bitboard
 * @param index the index of that bitboard to pop
 */
inline static constexpr void popBit(Bitboard &bb, Square sq) {
    if (getBit(bb, sq)) {
        bb ^= (1ull << sq);
    }
}
