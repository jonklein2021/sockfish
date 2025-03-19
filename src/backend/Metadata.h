#pragma once

#include <SFML/System/Vector2.hpp>
#include "PieceType.h"

/**
 * Metadata is comprised of information about the game state
 * that is cannot be read from the board position itself
 */
struct Metadata {
    bool blackRookAMoved;
    bool blackRookHMoved;
    bool whiteRookAMoved;
    bool whiteRookHMoved;
    bool whiteKingMoved;
    bool blackKingMoved;
    int movesSinceCapture;
    sf::Vector2<int> enPassantSquare;
    PieceType capturedPiece;
};