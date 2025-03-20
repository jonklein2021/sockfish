#pragma once

#include <cstdint>
#include <sstream>
#include <SFML/Graphics.hpp>

#include "Move.h"

/**
 * Stores the current state of the game in bitboard
 * representation. Each uint64_t represents the
 * instances of a certain piece on the board.
 */
struct BitBoard {
    uint64_t pieceBits[12]; // index into this with PieceType
    BitBoard() : pieceBits{0} {}
    BitBoard(const BitBoard &board) : pieceBits{0} {
        for (int i = 0; i < 12; i++) {
            pieceBits[i] = board.pieceBits[i];
        }
    }

    // Methods

    /**
     * Applies a move to the board
     * 
     * @param move the move to apply
     */
    void makeMove(const Move& move);

    
    /**
     * Undoes a move on the board
     * 
     * @param move the move to undo
     */
    void unmakeMove(const Move &move);
    
    /**
     * Used to test if a square of some bitboard is under attack by a certain color
     * 
     * @param square the square of the board to test
     * @param white true iff white is the attacker
     * @return true iff the square is under attack by the given color
     */
    bool attacked(sf::Vector2<int> square, bool white) const;

    /**
     * Checks if the position is drawn by insufficient material
     * 
     * @return true iff position is a draw
     */
    bool isDraw() const;
    
    /**
     * Returns the piece type at a given square
     * 
     * @param square the square to check
     */
    PieceType getPieceType(sf::Vector2<int> square) const;
    void print(PieceType p);
    void print();
    void prettyPrint(bool noFlip = true) const;
};

/**
 * Converts a BitBoard to a FEN string
 * 
 * Can be used to export the game state
 * 
 * @param board the BitBoard to convert
 * @return the FEN string representation of the BitBoard
 */
// std::string bitBoardToFen(const BitBoard& board);

// Debugging methods
void printU64(uint64_t n);
