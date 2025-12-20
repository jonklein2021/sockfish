#include "types.h"

class Board {
   private:
    // represents the location of each piece, indexed by
    // WP, ..., BK
    Bitboard pieces[12];

    // represents the joint occupancy of pieces on the board
    // indexes: WHITE, BLACK, BOTH, EMPTY
    Bitboard occupancies[4];

   public:
    Bitboard getPieces(Piece p) const;
    Bitboard getOccupancy(OccupancyType c) const;

    Piece pieceAt(Square sq) const;

    void addPiece(Piece p, Square sq);
    void removePiece(Piece p, Square sq);
    void movePiece(Piece p, Square from, Square to);
    void updateOccupancies();

    // do I need this?
    void clear();
};
