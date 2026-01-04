#include "ZobristHasher.h"

#include <random>

ZobristHasher::ZobristHasher(std::shared_ptr<Position> pos) {
    std::mt19937_64 generator;

    // initiallize array with random numbers
    for (int i = 0; i < ZOBRIST_ARRAY_SIZE; i++) {
        hashes[i] = generator();
    }

    // apply piece positions to hash
    for (Square sq : ALL_SQUARES) {
        Piece p = pos->pieceAt(sq);
        currentHash ^= hashes[getIndex(p, sq)];
    }

    // apply sideToMove hash only if black's turn
    if (pos->getSideToMove() == BLACK) {
        currentHash ^= hashes[getSideToMoveIndex()];
    }

    // apply castle rights status to hash
    const CastleRights cr = pos->getMetadata().castleRights;
    currentHash ^= hashes[getIndex(cr)];

    // apply en passant square to hash only if it exists
    if (pos->getMetadata().enPassantSquare != NO_SQ) {
        const int epFileIndex = fileOf(pos->getMetadata().enPassantSquare);
        currentHash ^= hashes[getIndex(epFileIndex)];
    }
}

uint64_t ZobristHasher::getHash() {
    return currentHash;
}

void ZobristHasher::update(Move m) {}
