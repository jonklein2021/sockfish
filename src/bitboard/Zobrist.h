#pragma once

#include "src/bitboard/PRNG.h"
#include "src/core/types.h"

// https://www.chessprogramming.org/Zobrist_Hashing
namespace Zobrist {

constexpr int PIECES = 12;
constexpr int SQUARES = 64;
constexpr int CASTLING = 16;
constexpr int EP_FILES = 8;

constexpr int SIDE_OFFSET = PIECES * SQUARES;
constexpr int CASTLE_OFFSET = SIDE_OFFSET + 1;
constexpr int EP_OFFSET = CASTLE_OFFSET + CASTLING;

// One number for each piece at each square
// One number to indicate the side to move is black
// Sixteen numbers to indicate the castling rights
// Eight numbers to indicate the file of a valid En passant square, if any
// (12 * 64) + 1 + 16 + 8 = 793 total
constexpr int ZOBRIST_SIZE = PIECES * SQUARES + 1 + CASTLING + EP_FILES;

inline constexpr std::array<uint64_t, ZOBRIST_SIZE> createZobristTable() {
    PRNG rnd;

    std::array<uint64_t, ZOBRIST_SIZE> t{};
    for (int i = 0; i < ZOBRIST_SIZE; i++) {
        t[i] = rnd.next();
    }

    return t;
}

inline constexpr std::array<uint64_t, ZOBRIST_SIZE> table = createZobristTable();

constexpr uint64_t getPieceSquareHash(Piece p, Square sq) {
    return table[p * SQUARES + sq];
}

constexpr uint64_t getSideToMoveHash() {
    return table[SIDE_OFFSET];
}

constexpr uint64_t getCastleRightsHash(CastleRights rights) {
    return table[CASTLE_OFFSET + rights];
}

constexpr uint64_t getEnPassantHash(int file) {
    return table[EP_OFFSET + file];
}

}  // namespace Zobrist
