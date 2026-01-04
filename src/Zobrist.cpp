#include "Zobrist.h"

#include <random>

namespace Zobrist {

std::array<uint64_t, ZOBRIST_SIZE> table;

// called once per program initialization
void init() {
    std::mt19937_64 rng(0xC0FFEE);  // fixed seed for reproducibility

    for (int i = 0; i < ZOBRIST_SIZE; i++) {
        table[i] = rng();
    }
}

uint64_t getPieceSquareHash(Piece p, Square sq) {
    return table[p * SQUARES + sq];
}

uint64_t getSideToMoveHash() {
    return table[SIDE_OFFSET];
}

uint64_t getCastleRightsHash(CastleRights rights) {
    return table[CASTLE_OFFSET + rights];
}

uint64_t getEnPassantHash(int file) {
    return table[EP_OFFSET + file];
}

}  // namespace Zobrist
