#pragma once

#include "types.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

// N.B: This struct is only used to make rendering easier; the internal system uses bitboard
// representation

/**
 * Represents a chess piece on the visual board
 */
struct VisualPiece {
    Piece piece;
    Square sq;
    std::unique_ptr<sf::Sprite> sprite;

    VisualPiece()
        : piece(NONE), sprite(nullptr) {}

    // Constructor with texture
    explicit VisualPiece(Piece piece, Square sq, const sf::Texture &texture)
        : piece(piece), sq(sq), sprite(std::make_unique<sf::Sprite>(texture)) {}
};

// food for thought: can I delete this struct altogether?
