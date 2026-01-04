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
    static constexpr int ZOBRIST_ARRAY_SIZE = 793;
    std::array<uint64_t, ZOBRIST_ARRAY_SIZE> hashes;

    // represents the hash of the current position
    uint64_t currentHash = 0ull;

    constexpr int getIndex(Piece p, Square sq) const {
        return p * sq;
    }

    // gets index of sideToMove. no argument because the hash's existence is enough to represent
    // whose turn it is
    constexpr int getSideToMoveIndex() const {
        return NO_PIECE * NO_SQ;
    }

    constexpr int getIndex(CastleRights cr) const {
        return NO_PIECE * NO_SQ + BLACK + cr;
    }

    constexpr int getIndex(int epFileIndex) const {
        return NO_PIECE * NO_SQ + BLACK + CASTLING_SZ + epFileIndex;
    }

   public:
    ZobristHasher(std::shared_ptr<Position> pos);
    uint64_t getHash();
    void update(Move m);
};
