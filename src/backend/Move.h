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
    
    /**
     * The type of piece that was moved
     */
    PieceType piece;

    /**
     * The type of piece that the pawn was promoted to or None if no promotion
     */
    PieceType promotionPiece;

    bool isCapture;
    bool isEnPassant;
    
    Move();
    Move(sf::Vector2<int> from, sf::Vector2<int> to, PieceType pieceMoved,
        bool isCapture = false, PieceType promotionPiece = None, bool isEnPassant = false);
    
    /**
     * N.B: This method ONLY compares the `from`, `to`, and `promotedPiece` members
     * of the Move struct. It does not consider `pieceMoved` or `isCapture`
     * 
     * @param other move to compare to
     */
    bool equals(const Move& other);

    /**
     * Convert the move to a readable string representation,
     * mainly used for debugging
     * 
     * @return string representation of the move
     */
    std::string to_string() const; // note: consider printing in a more stand format
};
