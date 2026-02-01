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

inline static constexpr Bitboard getBit(uint64_t bb, int index) {
    return bb & (1ull << index);
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

inline static constexpr uint16_t swap16(uint16_t v) {
    return (v >> 8) | (v << 8);
}

inline static constexpr uint32_t swap32(uint32_t v) {
    return ((v >> 24) & 0xff) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) |
           ((v << 24) & 0xff000000);
}

inline static constexpr uint64_t swap64(uint64_t v) {
    return ((uint64_t) swap32(v & 0xffffffff) << 32) | (uint64_t) swap32(v >> 32);
}

template<typename Fn>
constexpr void forEachSquare(Bitboard bb, Fn fn) {
    while (bb) {
        Square sq = Square(getLsbIndex(bb));
        fn(sq);
        bb &= bb - 1;
    }
}
