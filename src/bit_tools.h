#pragma once

#include "types.h"
#include <cstdint>

#include <SFML/System/Vector2.hpp>

/**
 * Returns index of the least significant bit
 *
 * Equivalently, the number of trailing 0s
 */
#define indexOfLs1b(bb) __builtin_ctzll(bb)

/**
 * Returns the bit at the given index
 *
 * @param bitboard the relevant bitboard
 * @param index the index of that bitboard to check
 * @return the bit at the given index
 */
inline static Bitboard getBit(Bitboard bb, int index) {
    return bb & (1ull << index);
}

/**
 * Sets the bit at the given index
 *
 * @param bb the relevant bitboard
 * @param index the index of that bitboard to set
 */
inline static void setBit(Bitboard &bb, int index) {
    bb |= (1ull << index);
}

/**
 * Converts a pair of coordinates to a bit
 *
 * @param x the x coordinate
 * @param y the y coordinate
 * @return the bit corresponding to the coordinates
 */
inline static Bitboard coordsToBit(int x, int y) {
    return 1ull << (y * 8 + x);
}

/**
 * Pops (removes and returns) the bit at the given index
 *
 * @param bb the relevant bitboard
 * @param index the index of that bitboard to pop
 */
inline static void popBit(Bitboard &bb, int index) {
    if (getBit(bb, index)) {
        bb ^= (1ull << index);
    }
}

/**
 * Converts a pair of coordinates (x, y) to an offset
 * in a bitboard (1 << offset is the bit in the bitboard)
 */
inline static uint8_t coordsToOffset(sf::Vector2<int> coords) {
    return coords.y * 8 + coords.x;
}

/**
 * Converts an offset (1 << offset is the bit in the bitboard)
 * to a pair of coordinates (x, y)
 */
inline static sf::Vector2<int> offsetToCoords(uint8_t offset) {
    return {offset % 8, offset / 8};
}

/**
 * Converts a pair of coordinates to a bit
 *
 * @param coords the coordinates to convert (x, y)
 * @return the bit corresponding to the coordinates
 */
inline static Bitboard coordsToBit(sf::Vector2<int> coords) {
    return coordsToBit(coords.x, coords.y);
}

/**
 * Converts a bit to a pair of coordinates
 *
 * @param bit the bit to convert
 * @return the corresponding coordinates (x, y)
 */
inline static sf::Vector2<int> bitToCoords(Bitboard bit) {
    int offset = indexOfLs1b(bit);
    return offsetToCoords(offset);
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
 * Computes the attacks of a pawn on the given square
 *
 * @param squareBit the bit denoting where the pawn is
 * @param white true iff the pawn is white
 * @return the bitboard of the pawn's attacks
 */
Bitboard computePawnAttacks(const Bitboard squareBit, const bool white);

/**
 * Computes the attacks of a knight on the given square
 *
 * @param squareBit the bit denoting where the knight is
 * @return the bitboard of the knight's attacks
 */
Bitboard computeKnightAttacks(const Bitboard squareBit);

/**
 * Computes the attacks of a bishop on the given square
 *
 * @param squareBit the bit denoting where the bishop is
 * @return the bitboard of the bishop's attacks
 */
Bitboard computeBishopAttacks(const Bitboard squareBit);

/**
 * Computes the attacks of a rook on the given square
 *
 * @param squareBit the bit denoting where the rook is
 * @return the bitboard of the rook's attacks
 */
Bitboard computeRookAttacks(const Bitboard squareBit);

/**
 * Computes the attacks of a queen on the given square
 *
 * @param squareBit the bit denoting where the queen is
 * @return the bitboard of the queen's attacks
 */
Bitboard computeQueenAttacks(const Bitboard squareBit);

/**
 * Computes the attacks of a king on the given square
 *
 * @param squareBit the bit denoting where the king is
 * @return the bitboard of the king's attacks
 */
Bitboard computeKingAttacks(const Bitboard squareBit);

/**
 * Converts the piece bitboards to a human-readable
 * board and prints it to stdout
 *
 * @param bb the bitboard to print
 */
void prettyPrintPosition(const Bitboard pieceBits[12], const bool noFlip);

void printBitboard(const Bitboard bb);

void printBoards(const Bitboard pieceBits[12]);
