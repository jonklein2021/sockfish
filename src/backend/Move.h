#include "types.h"

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
    
    Move() : from({0, 0}), to({0, 0}), pieceMoved(None), promotionPiece(None), isCapture(false) {}
    Move(sf::Vector2<int> from, sf::Vector2<int> to, PieceType pieceMoved, PieceType promotionPiece, bool isCapture)
        : from(from), to(to), pieceMoved(pieceMoved), promotionPiece(promotionPiece), isCapture(isCapture) {}
    Move(sf::Vector2<int> from, sf::Vector2<int> to, PieceType pieceMoved, bool isCapture)
        : from(from), to(to), pieceMoved(pieceMoved), promotionPiece(None), isCapture(isCapture) {}
    
    bool equals(const Move& other);
    std::string to_string() const;
};

