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
     * Total size = (8*12) + 8 + 4 + 4 + 1 + 1 + 1 + 1 + padding
     *            = 120 bytes after padding
     */
    struct Metadata {
        // maps pieces to the squares they control
        // N.B: consider taking this out of metadata to reduce data copy size
        std::array<Bitboard, NO_PIECE> attackTable;

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

    // returns false iff the player whose turn can capture its opponent's king
    constexpr bool isLegal() const {
        return !isAttacked(md.kingSquares[otherColor(sideToMove)], sideToMove);
    }

    constexpr Bitboard getPinnedPieces() const {
        return getPinnedPieces(sideToMove);
    }

    constexpr Bitboard getPinnedPieces(Color color) const {
        const Bitboard ourPieces = board.getOccupancy(color);
        const Bitboard potentialPinnerAttacks = getSlidingAttacks(otherColor(color));
        const Square kingSq = md.kingSquares[color];
        const Bitboard kingRadar = Magic::getQueenAttacks(kingSq, board.getOccupancies());

        return ourPieces & potentialPinnerAttacks & kingRadar;
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

    // Attack Table Methods

    struct AttackInfo {
        Piece attacker = NO_PIECE;
        Square attackerSq = NO_SQ;
    };

    // N.B: Consider making an AttackInfo struct to combine a lot of these methods
    constexpr bool isAttacked(Square sq, Color attacker) const {
        for (Piece p : COLOR_TO_PIECES[attacker]) {
            if (getBit(md.attackTable[p], sq)) {
                return true;
            }
        }
        return false;
    }

    constexpr int getNumAttackers(Square sq, Color attacker) const {
        int total = 0;
        for (Piece p : COLOR_TO_PIECES[attacker]) {
            if (getBit(md.attackTable[p], sq)) {
                total++;
            }
        }
        return total;
    }

    constexpr Bitboard getSideAttacksBB(Color attacker) const {
        Bitboard result = 0ull;
        for (Piece p : COLOR_TO_PIECES[attacker]) {
            result |= md.attackTable[p];
        }
        return result;
    }

    constexpr Bitboard getPieceAttacksBB(Piece attacker) const {
        return md.attackTable[attacker];
    }

    constexpr Bitboard getSlidingAttacks(Color attacker) const {
        return getPieceAttacksBB(ptToPiece(BISHOP, attacker)) |
               getPieceAttacksBB(ptToPiece(ROOK, attacker)) |
               getPieceAttacksBB(ptToPiece(QUEEN, attacker));
    }

    // N.B: This returns at most one piece, even if attackedSq is attacked by more than one piece of
    // the specified color.
    constexpr Piece getFirstAttacker(Square attackedSq, Color attacker) const {
        for (Piece p : COLOR_TO_PIECES[attacker]) {
            if (getBit(md.attackTable[p], attackedSq)) {
                return p;
            }
        }
        return NO_PIECE;
    }

    std::string toFenString() const;
};
