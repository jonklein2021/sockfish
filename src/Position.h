#pragma once

#include "Board.h"
#include "Move.h"

#include <cstdint>
#include <string>

/**
 * The game state includes the current position
 * whose turn it is, if each side can castle, etc.
 */
class Position {
   public:
    /**
     * Information about the game state
     * that is cannot be read from the board alone
     */
    struct Metadata {
        // 5-bit number [0:63] that represents the location
        // of the en passant square
        uint8_t enPassantBitOffset : 5;

        // used to check for 50 move rule
        uint8_t movesSinceCapture : 5;

        CastleRights castleRights;
    };

   private:
    Board board;

    Metadata md;

    Color sideToMove;

   public:
    // Constructors
    Position();

    Position(const std::string &fen);

    // Getters
    const Board &getBoard() const;

    Metadata getMetadata() const;

    Color getSideToMove() const;

    /**
     * Parses a FEN string and updates this object
     * with its contents
     */
    void parseFen(const std::string &fen);

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
};
