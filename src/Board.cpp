#include "Board.h"

#include "src/types.h"

#include <cstring>

Bitboard &Board::getPieces(Piece p) {
    return pieces[p];
}

Bitboard Board::getOccupancy(OccupancyType o) const {
    return occupancies[o];
}

Piece Board::pieceAt(Square sq) const {
    for (Piece p = WP; p <= BK; p = Piece(p + 1)) {
        if (pieces[p] & (1 << sq)) {
            return p;
        }
    }

    return NONE;
}

void Board::addPiece(Piece p, Square sq) {
    pieces[p] |= (1 << sq);
}

void Board::removePiece(Piece p, Square sq) {
    pieces[p] &= ~(1 << sq);
}

void Board::movePiece(Piece p, Square from, Square to) {
    pieces[p] ^= (1 << from) | (1 << to);
}

void Board::updateOccupancies() {
    occupancies[WHITE_OCCUPANCY] =
        pieces[WP] | pieces[WN] | pieces[WB] | pieces[WR] | pieces[WQ] | pieces[WK];
    occupancies[BLACK_OCCUPANCY] =
        pieces[BP] | pieces[BN] | pieces[BB] | pieces[BR] | pieces[BQ] | pieces[BK];
    occupancies[BOTH_OCCUPANCY] = occupancies[WHITE_OCCUPANCY] | occupancies[BLACK_OCCUPANCY];
    occupancies[EMPTY_OCCUPANCY] = ~occupancies[BOTH_OCCUPANCY];
}

void Board::clear() {
    std::memset(pieces, 0, 12 * sizeof(Bitboard));
}
