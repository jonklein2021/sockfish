#include <cstdint>
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
     * Used to test if a square of some bitboard is under attack by a certain color
     * 
     * @param square the square of the board to test
     * @param white true iff white is the attacker
     * @return true iff the square is under attack by the given color
     */
    void applyMove(const Move& move);
    bool attacked(sf::Vector2<int> square, bool white) const;
    void print(PieceType p);
    void print();
    std::string to_string();
};

/**
 * Converts a FEN string to a BitBoard
 * 
 * Only used in the initialization of the game
 * 
 * @param fen the FEN string to convert
 * @return the BitBoard representation of the FEN string
 */
BitBoard fenToBitBoard(const std::string& fen);

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
