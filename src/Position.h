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
        Square enPassantSquare;

        // the last move (that led to this postion) captured this piece
        Piece capturedPiece;

        // used to check for 50 move rule
        uint8_t movesSinceCapture;

        CastleRights castleRights;
    };

   private:
    Color sideToMove;

   public:
    Board board;

    Metadata md;

    // Constructors
    Position(const std::string &fen);

    // Getters
    constexpr const Board &getBoard() const {
        return board;
    }

    constexpr Bitboard getPieceBB(Piece p) const {
        return board.getPieceBB(p);
    }

    constexpr Piece pieceAt(Square sq) const {
        return board.pieceAt(sq);
    }

    constexpr Metadata getMetadata() const {
        return md;
    }

    constexpr Color getSideToMove() const {
        return sideToMove;
    }

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

    // TODO: Replace this with Zobrist hashing
    constexpr uint64_t hash() const {
        uint64_t res = 0;

        for (Piece p : ALL_PIECES) {
            res ^= (board.getPieceBB(p) << p);
        }

        res ^= (sideToMove << 12);
        res ^= (md.enPassantSquare << 13);
        res ^= (md.capturedPiece << 14);
        res ^= (md.movesSinceCapture << 15);
        res ^= (md.castleRights << 16);

        return res;
    }

    std::string toFenString();
};
