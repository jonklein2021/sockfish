#pragma once

#include "bit_tools.h"
#include "types.h"

#include <array>
#include <cstring>

class Board {
   private:
    // represents the location of each piece, indexed by
    // WP, ..., BK
    std::array<Bitboard, 12> pieceBBs;

    // represents the joint occupancy of pieces on the board
    // indexes: WHITE, BLACK, BOTH, EMPTY
    std::array<Bitboard, 4> occupancies;

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

    constexpr Bitboard getOccupancy(OccupancyType c) const {
        return occupancies[c];
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

    constexpr void removePiece(Piece p, Square sq) {
        assert(squareToPiece[sq] == p);
        squareToPiece[sq] = NO_PIECE;
        unsetBit(pieceBBs[p], sq);
    }

    constexpr void movePiece(Piece p, Square from, Square to) {
        // DEBUG
        // printf("movePiece(%s, %s, %s)\n", pieceNames[p].data(),
        //        squareToCoordinateString(from).c_str(), squareToCoordinateString(to).c_str());
        // printf("squareToPiece[%s] = %s\n", squareToCoordinateString(from).c_str(),
        //        pieceNames[squareToPiece[from]].data());
        // printf("squareToPiece[%s] = %s\n", squareToCoordinateString(to).c_str(),
        //        pieceNames[squareToPiece[to]].data());
        assert(squareToPiece[from] == p);
        assert(squareToPiece[to] == NO_PIECE);
        my_swap(squareToPiece[from], squareToPiece[to]);
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
        occupancies[WHITE_OCCUPANCY] =
            pieceBBs[WP] | pieceBBs[WN] | pieceBBs[WB] | pieceBBs[WR] | pieceBBs[WQ] | pieceBBs[WK];
        occupancies[BLACK_OCCUPANCY] =
            pieceBBs[BP] | pieceBBs[BN] | pieceBBs[BB] | pieceBBs[BR] | pieceBBs[BQ] | pieceBBs[BK];
        occupancies[BOTH_OCCUPANCY] = occupancies[WHITE_OCCUPANCY] | occupancies[BLACK_OCCUPANCY];
        occupancies[EMPTY_OCCUPANCY] = ~occupancies[BOTH_OCCUPANCY];
    }
};
