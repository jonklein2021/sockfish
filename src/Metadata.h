#pragma once

#include "types.h"

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <vector>

/**
 * Comprises information about the game state
 * that is cannot be read from the board position itself
 */
struct Metadata {
    // stores the hash of each state
    // 24 bytes (size, capacity, data ptr)
    std::vector<uint64_t> history;

    // 5-bit number [0:63] that represents the location
    // of the en passant square
    uint8_t enPassantBitOffset : 5;

    // used to check for 50 move rule
    uint8_t movesSinceCapture : 5;

    CastleRights castleRights;
};
