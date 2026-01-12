#pragma once

#include "Board.h"
#include "Move.h"
#include "src/types.h"

#include <string>

/**
 * The game state includes the current position
 * whose turn it is, if each side can castle, etc.
 */
class Position {
   public:
    /**
     * Information about the current position
     * that cannot be read from the board alone
     */
    struct Metadata {
        Square enPassantSquare = NO_SQ;

        // the last move (that led to this postion) captured this piece
        // we can use this to conditionally render a capture sound in the GUI
        Piece capturedPiece = NO_PIECE;

        // used to check for 50 move rule
        int movesSinceCapture = 0;

        // represents who current has what castling rights
        CastleRights castleRights = NO_CASTLING;

        // maintained by ZobristHasher
        uint64_t hash = 0ull;

        // maps pieces to the squares they control
        std::array<Bitboard, NO_PIECE> attackTable;
    };

   private:
    Color sideToMove;

    /**
     * Parses a FEN string and updates member vars, including metadata,
     * with its contents
     */
    void parseFen(const std::string &fen);

    void updatePieceAttacks(Piece p);

    void updateSideAttacks(Color c);

    void updateAllAttacks();

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

    constexpr uint64_t getHash() const {
        return md.hash;
    }

    // Other Methods

    /**
     * Makes a move in the current position
     *
     * @param move the move to apply to the game state
     * @return the metadata of the state before the move is made
     */
    Metadata makeMove(const Move &move);

    /**
     * Undoes a move in the current position
     * *
     * @param move the move to undo, assumed to be the most recent most played
     * @param prevMD the old metadata to restore
     */
    void unmakeMove(const Move &move, const Metadata &prevMD);

    constexpr bool isAttacked(Square sq, Color attacker) {
        for (Piece p : COLOR_TO_PIECES[attacker]) {
            if (getBit(md.attackTable[p], sq)) {
                return true;
            }
        }
        return false;
    }

    std::string toFenString() const;
};
