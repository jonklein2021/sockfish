#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "BitBoard.h"

/**
 * The game state includes the current position
 * whose turn it is, if each side can castle, etc.
 */
struct GameState {
    // board representation
    BitBoard board;

    // other game state variables
    bool whiteToMove;
    bool whiteKingMoved;
    bool blackKingMoved;
    bool whiteRookAMoved;
    bool whiteRookHMoved;
    bool blackRookAMoved;
    bool blackRookHMoved;

    // Constructors
    GameState();
                  
    GameState(const std::string &fen);

    GameState(BitBoard board, bool whiteToMove, bool whiteKingMoved, bool blackKingMoved,
              bool whiteRookAMoved, bool whiteRookHMoved, bool blackRookAMoved, bool blackRookHMoved);
    
    // Methods

    /**
     * Applys a move to the game state
     * 
     * @param move the move to apply to the game state
     */
    void applyMove(const Move& move);

    /**
     * Returns true iff the given square
     * is under attack by the opponent
     * 
     * @param square the square to check
     * 
     */
    bool underAttack(sf::Vector2<int> square) const;

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
};