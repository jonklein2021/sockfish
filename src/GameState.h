#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Metadata.h"
#include "Move.h"

/**
 * The game state includes the current position
 * whose turn it is, if each side can castle, etc.
 */
struct GameState {
private:
  // bitboards representing the locations of each piece
  uint64_t pieceBits[12];

  // bitboards representing the locations of each piece's attacks
  uint64_t pieceAttacks[12];

  // bitboards representing the joint occupancy of each side's pieces
  uint64_t occupancies[4];

public:
  // true iff it is white's turn to move
  bool whiteToMove;

  // game state information (who can castle, en passant square, etc.)
  Metadata md;

  // Constructors
  GameState();

  GameState(const std::string &fen);

  /**
   * Parses a FEN string and updates this object
   * with its contents
   */
  void parseFen(const std::string &fen);

  // Accessors for encapsulated bitboards (read-only)
  uint64_t pieceBit(PieceType p) const { return pieceBits[p]; }
  uint64_t pieceAttack(PieceType p) const { return pieceAttacks[p]; }
  uint64_t occupancy(int idx) const { return occupancies[idx]; }
  PieceType getCapturedPiece(const uint64_t toBit,
                             const std::vector<PieceType> &oppPieces) const;
  PieceType pieceAt(sf::Vector2<int> square) const;
  const uint64_t *pieceBitsData() const noexcept { return pieceBits; }
  const uint64_t *pieceAttacksData() const noexcept { return pieceAttacks; }
  const uint64_t *occupanciesData() const noexcept { return occupancies; }

  // Bitboard-only Methods
  uint64_t computeAllSidesAttacks() const;
  uint64_t computeAllSidesAttacks(bool white) const;
  uint64_t computePieceAttacks(PieceType piece) const;
  uint64_t computePawnAttacks(const uint64_t squareBit, const bool white) const;
  uint64_t computeKnightAttacks(const uint64_t squareBit) const;
  uint64_t computeBishopAttacks(const uint64_t squareBit) const;
  uint64_t computeRookAttacks(const uint64_t squareBit) const;
  uint64_t computeQueenAttacks(const uint64_t squareBit) const;
  uint64_t computeKingAttacks(const uint64_t squareBit) const;

  // Other Methods

  /**
   * Makes a move in the game state
   *
   * @param move the move to apply to the game state
   * @return the metadata of the state before the move is made
   */
  Metadata makeMove(const Move &move);

  /**
   * Undoes a move in the game state
   *
   * @param move the move to undo
   * @param prevMD the old metadata to restore
   */
  void unmakeMove(const Move &move, const Metadata &prevMD);

  /**
   * Returns true iff the given square
   * is under attack by the given side
   *
   * @param squareBit bit representing the square to check
   * @param white true iff the attacker is white
   */
  bool underAttack(const uint64_t squareBit, const bool white) const;

  /**
   * Returns true iff the given square
   * is under attack by the opponent
   *
   * @param squareBit bit representing the square to check
   * @return true iff the square is under attack by opponent
   */
  bool underAttack(const uint64_t squareBit) const;

  /**
   * @return true iff the game state is drawn
   * by insufficient material
   */
  bool insufficientMaterial() const;

  /**
   * @return true iff the game state is terminal
   */
  bool isTerminal() const;

  /**
   * Returns true iff the current player is in check
   */
  bool isCheck() const;

  /**
   * Helper function that checks if a move puts its king in check
   *
   * @param copy a reference to a copied game state to test
   * @param kingBit the bit representing the king's position
   * @param move the move to check
   * @param white true iff the move is made by white
   * @return true iff the move is legal
   */
  bool isMoveLegal(GameState &copy, const uint64_t kingBit, const Move &move,
                   bool white) const;

  /**
   * Return a list of all legal moves in some
   * game state
   *
   * @param state the current game state
   * @return a vector of all possible moves
   */
  std::vector<Move> generateMoves() const;

  /**
   * @return the hash of the GameState
   */
  uint64_t hash() const;

  /**
   * Print the game state to the console
   */
  void print() const;
};
