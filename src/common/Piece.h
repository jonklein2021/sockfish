#pragma once

#include <SFML/Graphics.hpp>

#include "PieceType.h"

// N.B: This struct is only used to make rendering easier; the search algorithm will use bitboard representation
struct Piece {
    PieceType type;
    sf::Vector2<int> position; // {x, y} where 0 <= x, y < 8
    sf::Sprite sprite;
};