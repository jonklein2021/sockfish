#pragma once

#include "types.h"

#include <string>

#include <SFML/System/Vector2.hpp>

/**
 * Moves are represented as a pair of two 2D vectors, where the
 * first vector is where the piece starts and the second vector
 * is where the piece ends up after the move.
 */
class Move {
private:
  uint16_t data;

public:
  Move();
  Move(Square from, Square to, PieceType pieceMoved,
       PieceType capturedPiece = None, PieceType promotedPiece = None,
       bool isKCastle = false, bool isQCastle = false,
       bool isEnPassant = false);

  Square from() const;

  Square to() const;

  PieceType pieceMoved() const;

  PieceType capturedPiece() const;

  PieceType promotedPiece() const;

  bool isEnPassant() const;

  /**
   * Convert the move to a readable string representation,
   * mainly used for debugging
   *
   * @return string representation of the move
   */
  std::string toString() const;

  bool operator==(const Move &other) const;

  bool operator!=(const Move &other) const;
};

Move coordsToMove(const std::string &input);

std::string moveToCoords(const Move &move);

bool validateCoords(const std::string &input);
