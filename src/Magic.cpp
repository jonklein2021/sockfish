#include "Magic.h"

#include "bit_tools.h"

namespace Magic {

std::array<Bitboard, NO_SQ> ROOK_MOVEMENT_MASKS;

std::array<Bitboard, NO_SQ> BISHOP_MOVEMENT_MASKS;

// 4096 = 2^12 = 2^max(ROOK_RELEVANT_BITS)
std::array<std::array<Bitboard, 4096>, NO_SQ> ROOK_ATTACKS;

// 512 = 2^9 = 2^max(BISHOP_RELEVANT_BITS)
std::array<std::array<Bitboard, 512>, NO_SQ> BISHOP_ATTACKS;

Bitboard computeFreeRookMoves(Square sq) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down
    for (int r = r0 + 1; r <= 6; r++) {
        allDstSqBB |= 1ull << xyToSquare(f0, r);
    }

    // up
    for (int r = r0 - 1; r >= 1; r--) {
        allDstSqBB |= 1ull << xyToSquare(f0, r);
    }

    // left
    for (int f = f0 - 1; f >= 1; f--) {
        allDstSqBB |= 1ull << xyToSquare(f, r0);
    }

    // right
    for (int f = f0 + 1; f <= 6; f++) {
        allDstSqBB |= 1ull << xyToSquare(f, r0);
    }

    return allDstSqBB;
}

Bitboard computeFreeBishopMoves(Square sq) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down right
    for (int r = r0 + 1, f = f0 + 1; r <= 6 && f <= 6; r++, f++) {
        allDstSqBB |= 1ull << xyToSquare(f, r);
    }

    // up left
    for (int r = r0 + 1, f = f0 - 1; r <= 6 && f >= 1; r++, f--) {
        allDstSqBB |= 1ull << xyToSquare(f, r);
    }

    // up right
    for (int r = r0 - 1, f = f0 + 1; r >= 1 && f <= 6; r--, f++) {
        allDstSqBB |= 1ull << xyToSquare(f, r);
    }

    // down left
    for (int r = r0 - 1, f = f0 - 1; r >= 1 && f >= 1; r--, f--) {
        allDstSqBB |= 1ull << xyToSquare(f, r);
    }

    return allDstSqBB;
}

Bitboard computeRookMovesNaively(Square sq, Bitboard blockers) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down
    for (int r = r0 + 1; r <= 7; r++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f0, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // up
    for (int r = r0 - 1; r >= 0; r--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f0, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // left
    for (int f = f0 - 1; f >= 0; f--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r0);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // right
    for (int f = f0 + 1; f <= 7; f++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r0);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    return allDstSqBB;
}

Bitboard computeBishopMovesNaively(Square sq, Bitboard blockers) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down right
    for (int r = r0 + 1, f = f0 + 1; r <= 7 && f <= 7; r++, f++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, f = f0 - 1; r <= 7 && f >= 0; r++, f--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // up right
    for (int r = r0 - 1, f = f0 + 1; r >= 0 && f <= 7; r--, f++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, f = f0 - 1; r >= 0 && f >= 0; r--, f--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    return allDstSqBB;
}

Bitboard setOccupancy(uint64_t index, Bitboard movementMask) {
    Bitboard occupancy = 0ull;
    int count = 0;

    while (movementMask) {
        Square square = Square(getLsbIndex(movementMask));

        if (index & (1 << count)) {
            occupancy |= (1ull << square);
        }

        count++;
        movementMask &= movementMask - 1;  // pop LSB
    }

    return occupancy;
}

void init() {
    // bishop
    for (Square sq : ALL_SQUARES) {
        const Bitboard bishopMoveMask = computeFreeBishopMoves(sq);

        // write mask to array
        BISHOP_MOVEMENT_MASKS[sq] = bishopMoveMask;

        // iterate over all 2^n variations, where n is the number of set 1s in this mask
        const uint64_t occupancyVariations = 1 << __builtin_popcountll(bishopMoveMask);
        for (uint64_t index = 0; index < occupancyVariations; index++) {
            // initialize attacks for this square at this magic index
            const uint64_t occupancy = setOccupancy(index, bishopMoveMask);
            const int numRelevantBits = 64 - BISHOP_RELEVANT_BITS[sq];
            const uint64_t magicIdx = (occupancy * BISHOP_MAGICS[sq]) >> numRelevantBits;
            BISHOP_ATTACKS[sq][magicIdx] = computeBishopMovesNaively(sq, occupancy);
        }
    }

    // rook (same steps)
    for (Square sq : ALL_SQUARES) {
        const Bitboard rookMoveMask = computeFreeRookMoves(sq);

        // write mask to array
        ROOK_MOVEMENT_MASKS[sq] = rookMoveMask;

        // iterate over all 2^n variations, where n is the number of set 1s in this mask
        const uint64_t occupancyVariations = 1 << __builtin_popcountll(rookMoveMask);
        for (uint64_t index = 0; index < occupancyVariations; index++) {
            // initialize attacks for this square at this magic index
            const Bitboard occupancy = setOccupancy(index, rookMoveMask);
            const int numRelevantBits = 64 - ROOK_RELEVANT_BITS[sq];
            const uint64_t magicIdx = (occupancy * ROOK_MAGICS[sq]) >> numRelevantBits;
            ROOK_ATTACKS[sq][magicIdx] = computeRookMovesNaively(sq, occupancy);
        }
    }
}

Bitboard getBishopAttacks(Square sq, Bitboard blockers) {
    // use blocker BB calculate magic index
    blockers &= BISHOP_MOVEMENT_MASKS[sq];
    blockers *= BISHOP_MAGICS[sq];
    blockers >>= 64 - BISHOP_RELEVANT_BITS[sq];

    return BISHOP_ATTACKS[sq][blockers];
}

Bitboard getRookAttacks(Square sq, Bitboard blockers) {
    // use blocker BB to calculate magic index
    blockers &= ROOK_MOVEMENT_MASKS[sq];
    blockers *= ROOK_MAGICS[sq];
    blockers >>= 64 - ROOK_RELEVANT_BITS[sq];

    return ROOK_ATTACKS[sq][blockers];
}

};  // namespace Magic
