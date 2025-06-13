#pragma once

#include <SFML/System/Vector2.hpp>
#include "PieceType.h"

/**
 * Metadata is comprised of information about the game state
 * that is cannot be read from the board position itself
 */
struct Metadata {
    /** 
     * 4 least significant bits signifies who can castle
     * 0b0000qkQK:
     * 
     * K = 1 iff white can kingside castle
     * Q = 1 iff white can queenside castle
     * k = 1 iff black can kingside castle
     * q = 1 iff black can queenside castle
     */
    uint8_t castleRights;

    // used to check for en passant
    uint64_t enPassantBit;

    // used to check for 50 move rule
    int movesSinceCapture;
    
    // stores the hash of each state
    std::vector<uint64_t> history;
};