#include "types.h"

#include <cstring>

class Board {
   private:
    // represents the location of each piece, indexed by
    // WP, ..., BK
    Bitboard pieces[12];

    // represents the joint occupancy of pieces on the board
    // indexes: WHITE, BLACK, BOTH, EMPTY
    Bitboard occupancies[4];

   public:
    // all getters return copies, not references

    constexpr Bitboard getPieces(Piece p) const {
        return pieces[p];
    }

    constexpr Bitboard getOccupancy(OccupancyType c) const {
        return occupancies[c];
    }

    // TODO: Add a squareToPiece table to speed this up
    constexpr Piece pieceAt(Square sq) const {
        for (Piece p : ALL_PIECES) {
            if (pieces[p] & (1ull << sq)) {
                return p;
            }
        }

        return NONE;
    }

    constexpr void addPiece(Piece p, Square sq) {
        pieces[p] |= (1ull << sq);
    }

    constexpr void removePiece(Piece p, Square sq) {
        pieces[p] &= ~(1ull << sq);
    }

    constexpr void movePiece(Piece p, Square from, Square to) {
        pieces[p] ^= (1ull << from) | (1ull << to);
    }

    constexpr void updateOccupancies() {
        occupancies[WHITE_OCCUPANCY] =
            pieces[WP] | pieces[WN] | pieces[WB] | pieces[WR] | pieces[WQ] | pieces[WK];
        occupancies[BLACK_OCCUPANCY] =
            pieces[BP] | pieces[BN] | pieces[BB] | pieces[BR] | pieces[BQ] | pieces[BK];
        occupancies[BOTH_OCCUPANCY] = occupancies[WHITE_OCCUPANCY] | occupancies[BLACK_OCCUPANCY];
        occupancies[EMPTY_OCCUPANCY] = ~occupancies[BOTH_OCCUPANCY];
    }

    constexpr void clear() {
        std::memset(pieces, 0, 12 * sizeof(Bitboard));
    }
};
