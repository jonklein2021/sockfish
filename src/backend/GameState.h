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

    // trivial
    bool whiteToMove;

    // used to check if castling is legal
    bool whiteKingMoved;
    bool blackKingMoved;
    bool whiteRookAMoved;
    bool whiteRookHMoved;
    bool blackRookAMoved;
    bool blackRookHMoved;

    // used to check for en passant
    sf::Vector2<int> enPassantSquare;

    // used to check for 50 move rule
    int movesSinceCapture;

    // TODO: use this to check for threefold repetition
    // std::vector<BitBoard> history;

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
     * @param metadata the metadata to restore
     */
    void unmakeMove(const Move &move, const Metadata &metadata);

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
};
