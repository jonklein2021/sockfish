#pragma once

#include "src/bitboard/bit_tools.h"
#include "src/core/types.h"

#include <array>
#include <cstring>

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
        clear();
    }

    // all getters return copies, not references
    constexpr Bitboard getPieceBB(Piece p) const {
        return pieceBBs[p];
    }

    constexpr Bitboard getOccupancy(Color c) const {
        return occupancies[c];
    }

    constexpr Bitboard getOccupancies() const {
        return getOccupancy(WHITE) | getOccupancy(BLACK);
    }

    constexpr Bitboard getEmptySquares() const {
        return ~getOccupancies();
    }

    constexpr Piece pieceAt(Square sq) const {
        return squareToPiece[sq];
    }

    constexpr void addPiece(Piece p, Square sq) {
        assert(squareToPiece[sq] == NO_PIECE);
        squareToPiece[sq] = p;
        setBit(pieceBBs[p], sq);
    }

    constexpr void removePiece(Piece p, Square sq) {
        assert(squareToPiece[sq] == p);
        squareToPiece[sq] = NO_PIECE;
        unsetBit(pieceBBs[p], sq);
    }

    constexpr void movePiece(Piece p, Square from, Square to) {
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

    void clear() {
        pieceBBs.fill(0ull);
        occupancies.fill(0ull);
        squareToPiece.fill(NO_PIECE);
    }
};
