#pragma once

#include "Position.h"
#include "types.h"

#include <SFML/System/Vector2.hpp>
#include <memory>

/**
 * Returns the least significant bit as a Square
 */
#define getLsbIndex(bb) __builtin_ctzll(bb)

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

/*** MOVE GENERATION TOOLS ***/

/*

    -- Bitboard Cheatsheet --

    y increasing
    |
    v   8   R N B Q K B N R
    |   7   P P P P P P P P
    v   6   . . . . . . . .
    |   5   . . . . . . . .
    v   4   . . . . . . . .
    |   3   . . . . . . . .
    v   2   p p p p p p p p
    |   1   r n b q k b n r
    v
            a b c d e f g h

    x increasing ->->->->->

    a8 is the 0th bit, h8 is the 7th bit
    a1 is the 56th bit, h1 is the 63rd bit

    Move one square up: x >> 8
    Move one square down: x << 8
    Move one square left: (x & not_file_a) >> 1
    Move one square right: (x & not_file_h) << 1

*/

/**
 * Converts the piece bitboards to a human-readable
 * board and prints it to stdout
 *
 * @param bb the bitboard to print
 */
void prettyPrintPosition(std::shared_ptr<Position> pos, bool flip = false);

void printBitboard(const Bitboard bb);

void printPieceValues(std::shared_ptr<Position> pos);
