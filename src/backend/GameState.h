#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Metadata.h"
#include "Move.h"
#include "bit_tools.h"
#include "constants.h"

/**
 * The game state includes the current position
 * whose turn it is, if each side can castle, etc.
 */
struct GameState {
    // bitboards representing the locations of each piece
    uint64_t pieceBits[12];

    // bitboards representing the locations of each piece's attacks
    uint64_t pieceAttacks[12];

    // bitboards representing the joint occupancy of each side's pieces
    uint64_t occupancies[4];

    // true iff it is white's turn to move
    bool whiteToMove;

    // game state information (who can castle, en passant square, etc.)
    Metadata md;

    // Constructors
    GameState();
                  
    GameState(const std::string &fen);
    
    // Bitboard-only Methods
    uint64_t computePieceAttacks(PieceType piece, uint64_t pieceBit) const;
    uint64_t computePawnAttacks(const uint64_t squareBit, const bool white) const;
    uint64_t computeKnightAttacks(const uint64_t squareBit) const;
    uint64_t computeBishopAttacks(const uint64_t squareBit) const;
    uint64_t computeRookAttacks(const uint64_t squareBit) const;
    uint64_t computeQueenAttacks(const uint64_t squareBit) const;
    uint64_t computeKingAttacks(const uint64_t squareBit) const;

    PieceType getCapturedPiece(const uint64_t toBit, const std::vector<PieceType> &oppPieces) const;
    PieceType pieceAt(sf::Vector2<int> square) const;
    
    // Other Methods

    /**
     * Makes a move in the game state
     * 
     * @param move the move to apply to the game state
     * @return the metadata of the state before the move is made
     */
    Metadata makeMove(const Move& move);

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
     * @param square the square to check
     * @param white true iff the attacker is white
     */
    bool underAttack(const sf::Vector2<int> &square, const bool white) const;

    /**
     * Returns true iff the given square
     * is under attack by the opponent
     * 
     * @param square the square to check
     * @return true iff the square is under attack by opponent
     */
    bool underAttack(const sf::Vector2<int> &square) const;

    /**
     * @return true iff the game state is terminal
     */
    bool isTerminal() const;

    /**
     * Returns true iff the current player is in check
     */
    bool isCheck() const;

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
