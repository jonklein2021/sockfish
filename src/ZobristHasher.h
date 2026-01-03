#include "Position.h"

#include <memory>

// note: consider making this a methodless struct inside Position

// https://www.chessprogramming.org/Zobrist_Hashing
class ZobristHasher {
   private:
    // One number for each piece at each square
    // One number to indicate the side to move is black
    // Sixteen numbers to indicate the castling rights
    // Eight numbers to indicate the file of a valid En passant square, if any
    // 12*64 + 1 + 16 + 8 = 793 total
    std::array<uint64_t, 793> hashes;
    uint64_t currentHash;

    int getIndex(Piece p, Square sq, CastleRights cr, int epFile);

   public:
    ZobristHasher(std::shared_ptr<Position> pos);
    uint64_t getHash();
    void update(Move m);
};
