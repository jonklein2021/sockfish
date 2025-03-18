#pragma once

#include "constants.h"

#include <SFML/Graphics.hpp>

/**
 * Moves are represented as a pair of two 2D vectors, where the
 * first vector is where the piece starts and the second vector
 * is where the piece ends up after the move.
 */
struct Move {
    sf::Vector2<int> from;
    sf::Vector2<int> to;
    PieceType pieceMoved;
    PieceType promotionPiece;
    bool isCapture;
    bool isEnPassant;
    
    Move();
    Move(sf::Vector2<int> from, sf::Vector2<int> to, PieceType pieceMoved,
        bool isCapture = false, PieceType promotionPiece = None, bool isEnPassant = false);
    
    bool equals(const Move& other);
    std::string to_string() const;
};

