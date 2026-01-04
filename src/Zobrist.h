#pragma once

#include "types.h"

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

// declared in ZobristHasher.cpp
extern std::array<uint64_t, ZOBRIST_SIZE> table;

void init();

uint64_t getSideToMoveHash();
uint64_t getPieceSquareHash(Piece p, Square sq);
uint64_t getCastleRightsHash(CastleRights cr);
uint64_t getEnPassantHash(int epFileIndex);
}  // namespace Zobrist
