#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

#include <SFML/Graphics.hpp>

/**
 * Represents each the type of piece;
 * 
 * Useful for simplifying moves
 */
enum PieceType {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

/**
 * Moves are represented as a pair of two 2D vectors, where the
 * first vector is where the piece starts and the second vector
 * is where the piece ends up after the move.
 */
struct Move {
    sf::Vector2<int> from;
    sf::Vector2<int> to;
    PieceType pieceMoved;
    bool isCapture;
    Move() : from({0, 0}), to({0, 0}), pieceMoved(BK), isCapture(false) {}
    Move(sf::Vector2<int> from, sf::Vector2<int> to, PieceType pieceMoved, bool isCapture)
        : from(from), to(to), pieceMoved(pieceMoved), isCapture(isCapture) {}
    bool equals(const Move& other);
};

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
};

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
    GameState() : board(), whiteToMove(true), whiteKingMoved(false), blackKingMoved(false),
                  whiteRookAMoved(false), whiteRookHMoved(false), blackRookAMoved(false), blackRookHMoved(false) {}
    GameState(BitBoard board, bool whiteToMove, bool whiteKingMoved, bool blackKingMoved,
              bool whiteRookAMoved, bool whiteRookHMoved, bool blackRookAMoved, bool blackRookHMoved)
        : board(board), whiteToMove(whiteToMove), whiteKingMoved(whiteKingMoved), blackKingMoved(blackKingMoved),
          whiteRookAMoved(whiteRookAMoved), whiteRookHMoved(whiteRookHMoved), blackRookAMoved(blackRookAMoved), blackRookHMoved(blackRookHMoved) {}
    
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
void print(const BitBoard &board, PieceType p);
void print(const BitBoard &board);
std::string to_string(const Move &move);
void prettyPrint(const BitBoard &board);
