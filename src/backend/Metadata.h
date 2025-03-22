#pragma once

#include <SFML/System/Vector2.hpp>
#include "PieceType.h"

/**
 * Metadata is comprised of information about the game state
 * that is cannot be read from the board position itself
 */
struct Metadata {
    bool whiteKCastleRights;
    bool whiteQCastleRights;
    bool blackKCastleRights;
    bool blackQCastleRights;

    // used to check for en passant
    sf::Vector2<int> enPassantSquare;

    // used to check for 50 move rule
    int movesSinceCapture;
    
    // stores the hash of each state
    std::vector<uint64_t> history;
};