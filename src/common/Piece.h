#pragma once

#include <SFML/Graphics.hpp>

#include "PieceType.h"

// N.B: This struct is only used to make rendering easier; the internal system uses bitboard representation

/**
 * Represents a chess piece on the visual board
 */
struct Piece {
    PieceType type;
    sf::Vector2<int> position; // {x, y} where 0 <= x, y < 8
    sf::Sprite sprite;
};

// food for thought: can I delete this struct altogether?
