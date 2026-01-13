#pragma once

#include "types.h"

// minimal swap implementation so I don't need to include the entire utility header
template<typename T>
inline static constexpr void my_swap(T &x, T &y) {
    T temp = x;
    x = y;
    y = temp;
}

/**
 * Returns the least significant bit as a Square
 */
#define getLsbIndex(bb) __builtin_ctzll(bb)

// constexpr popcount implementation based on Stanford's bit twiddling hacks:
// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
inline static constexpr int my_popcount(uint64_t x) {
    x -= (x >> 1) & 0x5555555555555555ULL;
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    x = (x + (x >> 8));
    x = (x + (x >> 16));
    x = (x + (x >> 32));
    return x & 0x000000000000007FULL;  // max count is 64 (fits in 7 bits)
}

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
inline static constexpr Bitboard coordsToBit(int x, int y) {
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
