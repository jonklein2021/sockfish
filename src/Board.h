#pragma once

#include "bit_tools.h"
#include "types.h"

#include <array>
#include <cstring>
#include <sstream>

class Board {
   private:
    // represents the location of each piece, indexed by
    // WP, ..., BK
    std::array<Bitboard, NO_PIECE> pieceBBs;

    // represents the joint occupancy of pieces on the board
    // indexes: WHITE, BLACK
    std::array<Bitboard, 2> occupancies;

    // maps each square to the piece residing on it
    std::array<Piece, NO_SQ> squareToPiece;

   public:
    Board() {
        pieceBBs.fill(0ull);
        occupancies.fill(0ull);
        squareToPiece.fill(NO_PIECE);
    }

    // all getters return copies, not references
    constexpr Bitboard getPieceBB(Piece p) const {
        return pieceBBs[p];
    }

    inline constexpr Bitboard getOccupancy(Color c) const {
        return occupancies[c];
    }

    inline constexpr Bitboard getOccupancies() const {
        return getOccupancy(WHITE) | getOccupancy(BLACK);
    }

    inline constexpr Bitboard getEmptySquares() const {
        return ~getOccupancies();
    }

    constexpr Piece pieceAt(Square sq) const {
        return squareToPiece[sq];
    }

    constexpr void addPiece(Piece p, Square sq) {
        assert(squareToPiece[sq] == NO_PIECE);
        squareToPiece[sq] = p;
        // printf("squareToPiece[%s] = %s\n", squareToCoordinateString(sq).c_str(),
        //        pieceNames[squareToPiece[sq]].data());
        setBit(pieceBBs[p], sq);
    }

    void removePiece(Piece p, Square sq) {
        // if (pieceToPT(p) == KING) {
        //     printf("removePiece(%s, %s)\n", PIECE_NAMES[p].data(),
        //            squareToCoordinateString(sq).c_str());
        //     for (Piece p : ALL_PIECES) {
        //         printf("%s bitboard:\n", PIECE_NAMES[p].data());
        //         std::ostringstream out;
        //         for (int rank = 0; rank < 8; rank++) {
        //             out << (8 - rank) << "  ";
        //             for (int file = 0; file < 8; file++) {
        //                 out << (getBit(getPieceBB(p), xyToSquare(file, rank)) ? "1" : "0") << "
        //                 ";
        //             }
        //             out << "\n";
        //         }
        //
        //         out << "\n   a b c d e f g h\n";
        //         printf("%s\n", out.str().c_str());
        //     }
        //     exit(1);
        // }

        assert(squareToPiece[sq] == p);
        squareToPiece[sq] = NO_PIECE;
        unsetBit(pieceBBs[p], sq);
    }

    void movePiece(Piece p, Square from, Square to) {
        // DEBUG
        // if (squareToPiece[from] != p) {
        //     printf("movePiece(%s, %s, %s)\n", PIECE_NAMES[p].data(),
        //            squareToCoordinateString(from).c_str(), squareToCoordinateString(to).c_str());
        //     printf("squareToPiece[%s] = %s\n", squareToCoordinateString(from).c_str(),
        //            PIECE_NAMES[squareToPiece[from]].data());
        //     printf("squareToPiece[%s] = %s\n", squareToCoordinateString(to).c_str(),
        //            PIECE_NAMES[squareToPiece[to]].data());
        //
        //     printf("%s bitboard:\n", PIECE_NAMES[p].data());
        //     std::ostringstream out;
        //     for (int rank = 0; rank < 8; rank++) {
        //         out << (8 - rank) << "  ";
        //         for (int file = 0; file < 8; file++) {
        //             out << (getBit(getPieceBB(p), xyToSquare(file, rank)) ? "1" : "0") << " ";
        //         }
        //         out << "\n";
        //     }
        //
        //     out << "\n   a b c d e f g h\n";
        //     printf("%s\n", out.str().c_str());
        // }
        assert(squareToPiece[from] == p);
        assert(squareToPiece[to] == NO_PIECE);
        squareToPiece[from] = NO_PIECE;
        squareToPiece[to] = p;
        pieceBBs[p] ^= (1ull << from) | (1ull << to);
    }

    constexpr void swapPiece(Square sq, Piece from, Piece to) {
        assert(squareToPiece[sq] == from);
        squareToPiece[sq] = to;
        unsetBit(pieceBBs[from], sq);
        setBit(pieceBBs[to], sq);
    }

    // allows for updating this table only when strictly necessary
    constexpr void updateOccupancies() {
        occupancies[WHITE] =
            pieceBBs[WP] | pieceBBs[WN] | pieceBBs[WB] | pieceBBs[WR] | pieceBBs[WQ] | pieceBBs[WK];
        occupancies[BLACK] =
            pieceBBs[BP] | pieceBBs[BN] | pieceBBs[BB] | pieceBBs[BR] | pieceBBs[BQ] | pieceBBs[BK];
    }
};
