#pragma once

#include "src/bitboard/Magic.h"
#include "src/core/Board.h"
#include "src/core/Move.h"
#include "src/core/types.h"

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
     *
     * Total size = 8 + 4 + 4 + 1 + 1 + 1 + 1 + padding
     *            = 24 bytes after padding
     */
    struct Metadata {
        // maintained by ZobristHasher
        uint64_t hash = 0ull;

        // the last move (that led to this postion) captured this piece
        // we can use this to conditionally render a capture sound in the GUI
        Piece capturedPiece = NO_PIECE;

        // used to check for 50 move rule
        int movesSinceCapture = 0;

        // represents who current has what castling rights
        CastleRights castleRights = NO_CASTLING;

        Square enPassantSquare = NO_SQ;

        std::array<Square, 2> kingSquares = {NO_SQ, NO_SQ};
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

    Board getBoardCopy() const;

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

    constexpr Square getKingSquare(Color c) const {
        return md.kingSquares[c];
    }

    constexpr Square getEpSquare() const {
        return md.enPassantSquare;
    }

    constexpr uint64_t getHash() const {
        return md.hash;
    }

    // Other Methods

    // Used to detect check and ensure legal castling
    template<Color attacker>
    constexpr bool isAttacked(Square sq) const {
        constexpr Color defender = otherColor(attacker);
        constexpr std::array<Piece, 6> enemyPieces = {
            ptToPiece(PAWN, attacker), ptToPiece(KNIGHT, attacker), ptToPiece(BISHOP, attacker),
            ptToPiece(ROOK, attacker), ptToPiece(QUEEN, attacker),  ptToPiece(KING, attacker),
        };

        const Bitboard occ = board.getOccupancies();

        // Diagonal Attacks: bishop + queen
        const Bitboard diagonalAttacks = Magic::getBishopAttacks(sq, occ);
        if (diagonalAttacks & (getPieceBB(enemyPieces[BISHOP]) | getPieceBB(enemyPieces[QUEEN]))) {
            return true;
        }

        // Lateral Attacks: rook + queen
        const Bitboard lateralAttacks = Magic::getRookAttacks(sq, occ);
        if (lateralAttacks & (getPieceBB(enemyPieces[ROOK]) | getPieceBB(enemyPieces[QUEEN]))) {
            return true;
        }

        // Knight
        if (KNIGHT_MASKS[sq] & getPieceBB(enemyPieces[KNIGHT])) {
            return true;
        }

        // Pawn
        if (PAWN_ATTACK_MASKS[defender][sq] & getPieceBB(enemyPieces[PAWN])) {
            return true;
        }

        // King
        if (KING_MASKS[sq] & getPieceBB(enemyPieces[KING])) {
            return true;
        }

        return false;
    }

    constexpr bool isAttacked(Square sq, Color attacker) const {
        return attacker == WHITE ? isAttacked<WHITE>(sq) : isAttacked<BLACK>(sq);
    }

    // returns false iff a king can be captured in the current position
    constexpr bool isLegal() const {
        return !isAttacked(md.kingSquares[otherColor(sideToMove)], sideToMove);
    }

    /**
     * Parses a FEN string and updates member vars, including metadata,
     * with its contents
     */
    void parseFen(const std::string &fen);

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

    std::string toFenString() const;
};
