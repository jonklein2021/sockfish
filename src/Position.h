#pragma once

#include "Board.h"
#include "Move.h"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
    constexpr uint64_t hash() {
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

    void printMoveList(const std::vector<Move> &moveList) const {
        // srcSq -> {Piece, {dstSq0, ..., dstSqN}}
        std::array<std::pair<Piece, std::vector<Square>>, 64> movesFromSquares;

        // group moves by the piece moved
        for (const Move &m : moveList) {
            Piece moved = board.pieceAt(m.getFromSquare());
            movesFromSquares[m.getFromSquare()].first = moved;
            movesFromSquares[m.getFromSquare()].second.push_back(m.getToSquare());
        }

        // build output string
        std::ostringstream ss;
        ss << std::to_string(moveList.size()) + " Moves:\n";
        for (Square sq = a8; sq <= h1; sq = Square(sq + 1)) {
            if (movesFromSquares[sq].second.empty()) {
                continue;
            }
            Piece moved = movesFromSquares[sq].first;
            ss << pieceNames[moved] << " on " << squareToCoordinateString(sq) << " -> { ";

            for (Square dst : movesFromSquares[sq].second) {
                ss << squareToCoordinateString(dst) << " ";
            }

            ss << "}\n";
        }

        std::cout << ss.str();
    }
};
