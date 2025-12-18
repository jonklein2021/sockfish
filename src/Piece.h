#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

// N.B: This struct is only used to make rendering easier; the internal system uses bitboard
// representation

/**
 * Represents a chess piece on the visual board
 */
struct Piece {
    PieceType type;
    sf::Vector2<int> position;  // {x, y} where 0 <= x, y < 8
    std::unique_ptr<sf::Sprite> sprite;

    // Default constructor - creates empty piece
    Piece() : type(None), position({-1, -1}), sprite(nullptr) {}

    // Constructor with texture
    explicit Piece(PieceType type, sf::Vector2<int> position, const sf::Texture &texture)
        : type(type), position(position), sprite(std::make_unique<sf::Sprite>(texture)) {}
};

// food for thought: can I delete this struct altogether?
