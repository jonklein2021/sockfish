#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>

#include "constants.h"

#include <SFML/System/Vector2.hpp>

// useful for converting a square to a bitboard index
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

/**
 * Returns index of the least significant bit
 * 
 * Equivalently, the number of trailing 0s
 */
#define indexOfLs1b(bitboard) __builtin_ctzll(bitboard)

/**
 * Returns the bit at the given index
 * 
 * @param bitboard the relevant bitboard
 * @param index the index of that bitboard to check
 * @return the bit at the given index
 */
inline static uint64_t getBit(uint64_t bitboard, int index) {
    return bitboard & (1ull << index);
}

/**
 * Sets the bit at the given index
 * 
 * @param bitboard the relevant bitboard
 * @param index the index of that bitboard to set
 */
inline static void setBit(uint64_t& bitboard, int index) {
    bitboard |= (1ull << index);
}


/**
 * Converts a pair of coordinates to a bit
 * 
 * @param x the x coordinate
 * @param y the y coordinate
 * @return the bit corresponding to the coordinates
 */
inline static uint64_t coordsToBit(int x, int y) {
    return (1ull << (y * 8 + x));
}

/**
 * Pops (removes and returns) the bit at the given index
 * 
 * @param bitboard the relevant bitboard
 * @param index the index of that bitboard to pop
 * @return the bit at the given index
 */
inline static void popBit(uint64_t& bitboard, int index) {
    if (getBit(bitboard, index)) {
        bitboard ^= (1ull << index);
    }
}

/**
 * Converts a pair of coordinates to a bit
 * 
 * @param coords the coordinates to convert
 * @return the bit corresponding to the coordinates
 */
inline static uint64_t coordsToBit(sf::Vector2<int> coords) {
    return coordsToBit(coords.x, coords.y);
}

/**
 * Converts a bit to a pair of coordinates
 * 
 * @param bit the bit to convert
 * @return the corresponding coordinates
 */
inline static sf::Vector2<int> bitToCoords(uint64_t bit) {
    int index = __builtin_ctzll(bit);
    return {index % 8, index / 8};
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


// Bitmasks that zero out specific files or ranks
// Useful for preventing pieces from going out of bounds

const uint64_t not_file_a = 18374403900871474942ull;
const uint64_t not_file_h = 9187201950435737471ull;
const uint64_t not_file_gh = 4557430888798830399ull;
const uint64_t not_file_ab = 18229723555195321596ull;
const uint64_t not_rank_1 = 72057594037927935ull;
const uint64_t not_rank_12 = 281474976710655ull;
const uint64_t not_rank_8 = 18446744073709551360ull;
const uint64_t not_rank_78 = 18446744073709486080ull;


// Bitmasks for pawn moves
const uint64_t rank1 = 18374686479671623680ull;
const uint64_t rank2 = 71776119061217280ull;
const uint64_t rank7 = 65280ull;
const uint64_t rank8 = 255ull;

/**
 * Computes the attacks of a pawn on the given square
 * 
 * @param squareBit the bit denoting where the pawn is
 * @param white true iff the pawn is white
 * @return the bitboard of the pawn's attacks
 */
uint64_t computePawnAttacks(const uint64_t squareBit, const bool white);

/**
 * Computes the attacks of a knight on the given square
 * 
 * @param squareBit the bit denoting where the knight is
 * @return the bitboard of the knight's attacks
 */
uint64_t computeKnightAttacks(const uint64_t squareBit);

/**
 * Computes the attacks of a bishop on the given square
 * 
 * @param squareBit the bit denoting where the bishop is
 * @return the bitboard of the bishop's attacks
 */
uint64_t computeBishopAttacks(const uint64_t squareBit);

/**
 * Computes the attacks of a rook on the given square
 * 
 * @param squareBit the bit denoting where the rook is
 * @return the bitboard of the rook's attacks
 */
uint64_t computeRookAttacks(const uint64_t squareBit);

/**
 * Computes the attacks of a queen on the given square
 * 
 * @param squareBit the bit denoting where the queen is
 * @return the bitboard of the queen's attacks
 */
uint64_t computeQueenAttacks(const uint64_t squareBit);

/**
 * Computes the attacks of a king on the given square
 * 
 * @param squareBit the bit denoting where the king is
 * @return the bitboard of the king's attacks
 */
uint64_t computeKingAttacks(const uint64_t squareBit);

/**
 * Converts the piece bitboards to a human-readable
 * board and prints it to stdout
 * 
 * @param pieceBits the bitboard to print
 */
void prettyPrintPosition(const uint64_t pieceBits[12], const bool noFlip);

void printBitboard(const uint64_t bitboard);

void printBoards(const uint64_t pieceBits[12]);
