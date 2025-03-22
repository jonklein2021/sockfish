#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "BitBoard.h"
#include "Metadata.h"

/**
 * The game state includes the current position
 * whose turn it is, if each side can castle, etc.
 */
struct GameState {
    // board representation
    BitBoard board;

    bool whiteToMove;

    // game state information
    Metadata md;

    // Constructors
    GameState();
                  
    GameState(const std::string &fen);
    
    // Methods

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
     * is under attack by the opponent
     * 
     * @param square the square to check
     * 
     */
    bool underAttack(sf::Vector2<int> square) const;

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
