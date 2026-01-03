#include "ZobristHasher.h"

#include <random>

ZobristHasher::ZobristHasher(std::shared_ptr<Position> pos) {
    // initiallize array with random numbers
    std::mt19937_64 generator;

    // compute currentHash of pos using array
}

int getIndex(Piece p, Square sq, Color toMove, CastleRights cr, int epFile) {
    return p * sq + toMove + cr + epFile;
}

uint64_t ZobristHasher::getHash() {
    return currentHash;
}

void ZobristHasher::update(Move m) {}
