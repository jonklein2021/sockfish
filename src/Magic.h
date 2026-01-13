#include "types.h"

namespace Magic {
static constexpr std::array<uint64_t, NO_SQ> ROOK_MAGICS = {
    0x8a80104000800020ull, 0x140002000100040ull,  0x2801880a0017001ull,  0x100081001000420ull,
    0x200020010080420ull,  0x3001c0002010008ull,  0x8480008002000100ull, 0x2080088004402900ull,
    0x800098204000ull,     0x2024401000200040ull, 0x100802000801000ull,  0x120800800801000ull,
    0x208808088000400ull,  0x2802200800400ull,    0x2200800100020080ull, 0x801000060821100ull,
    0x80044006422000ull,   0x100808020004000ull,  0x12108a0010204200ull, 0x140848010000802ull,
    0x481828014002800ull,  0x8094004002004100ull, 0x4010040010010802ull, 0x20008806104ull,
    0x100400080208000ull,  0x2040002120081000ull, 0x21200680100081ull,   0x20100080080080ull,
    0x2000a00200410ull,    0x20080800400ull,      0x80088400100102ull,   0x80004600042881ull,
    0x4040008040800020ull, 0x440003000200801ull,  0x4200011004500ull,    0x188020010100100ull,
    0x14800401802800ull,   0x2080040080800200ull, 0x124080204001001ull,  0x200046502000484ull,
    0x480400080088020ull,  0x1000422010034000ull, 0x30200100110040ull,   0x100021010009ull,
    0x2002080100110004ull, 0x202008004008002ull,  0x20020004010100ull,   0x2048440040820001ull,
    0x101002200408200ull,  0x40802000401080ull,   0x4008142004410100ull, 0x2060820c0120200ull,
    0x1001004080100ull,    0x20c020080040080ull,  0x2935610830022400ull, 0x44440041009200ull,
    0x280001040802101ull,  0x2100190040002085ull, 0x80c0084100102001ull, 0x4024081001000421ull,
    0x20030a0244872ull,    0x12001008414402ull,   0x2006104900a0804ull,  0x1004081002402ull,
};

static constexpr std::array<uint64_t, NO_SQ> BISHOP_MAGICS = {
    0x40040844404084ull,   0x2004208a004208ull,   0x10190041080202ull,   0x108060845042010ull,
    0x581104180800210ull,  0x2112080446200010ull, 0x1080820820060210ull, 0x3c0808410220200ull,
    0x4050404440404ull,    0x21001420088ull,      0x24d0080801082102ull, 0x1020a0a020400ull,
    0x40308200402ull,      0x4011002100800ull,    0x401484104104005ull,  0x801010402020200ull,
    0x400210c3880100ull,   0x404022024108200ull,  0x810018200204102ull,  0x4002801a02003ull,
    0x85040820080400ull,   0x810102c808880400ull, 0xe900410884800ull,    0x8002020480840102ull,
    0x220200865090201ull,  0x2010100a02021202ull, 0x152048408022401ull,  0x20080002081110ull,
    0x4001001021004000ull, 0x800040400a011002ull, 0xe4004081011002ull,   0x1c004001012080ull,
    0x8004200962a00220ull, 0x8422100208500202ull, 0x2000402200300c08ull, 0x8646020080080080ull,
    0x80020a0200100808ull, 0x2010004880111000ull, 0x623000a080011400ull, 0x42008c0340209202ull,
    0x209188240001000ull,  0x400408a884001800ull, 0x110400a6080400ull,   0x1840060a44020800ull,
    0x90080104000041ull,   0x201011000808101ull,  0x1a2208080504f080ull, 0x8012020600211212ull,
    0x500861011240000ull,  0x180806108200800ull,  0x4000020e01040044ull, 0x300000261044000aull,
    0x802241102020002ull,  0x20906061210001ull,   0x5a84841004010310ull, 0x4010801011c04ull,
    0xa010109502200ull,    0x4a02012000ull,       0x500201010098b028ull, 0x8040002811040900ull,
    0x28000010020204ull,   0x6000020202d0240ull,  0x8918844842082200ull, 0x4010011029020020ull,
};

static constexpr std::array<int, NO_SQ> ROOK_RELEVANT_BITS = {
    12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10,
    10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10,
    10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12};

static constexpr std::array<int, NO_SQ> BISHOP_RELEVANT_BITS = {
    6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6};

static std::array<Bitboard, NO_SQ> ROOK_MOVEMENT_MASKS;

static std::array<Bitboard, NO_SQ> BISHOP_MOVEMENT_MASKS;

static std::array<std::array<Bitboard, 4096>, NO_SQ> ROOK_ATTACKS;

static std::array<std::array<Bitboard, 512>, NO_SQ> BISHOP_ATTACKS;

constexpr Bitboard computeRookMovementMask(Square sq, Bitboard blockers = 0ull) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down
    for (int r = r0 + 1; r <= 6; r++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f0, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // up
    for (int r = r0 - 1; r >= 1; r--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f0, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // left
    for (int f = f0 - 1; f >= 1; f--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r0);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // right
    for (int f = f0 + 1; f <= 6; f++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r0);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    return allDstSqBB;
}

constexpr Bitboard computeBishopMovementMask(Square sq, Bitboard blockers = 0ull) {
    Bitboard allDstSqBB = 0ull;
    const int r0 = rankOf(sq), f0 = fileOf(sq);

    // down right
    for (int r = r0 + 1, f = f0 + 1; r <= 6 && f <= 6; r++, f++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, f = f0 - 1; r <= 6 && f >= 1; r++, f--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // up right
    for (int r = r0 - 1, f = f0 + 1; r >= 1 && f <= 6; r--, f++) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, f = f0 - 1; r >= 1 && f >= 1; r--, f--) {
        Bitboard dstSqBB = 1ull << xyToSquare(f, r);
        allDstSqBB |= dstSqBB;
        if (blockers & dstSqBB) {
            break;
        }
    }

    return allDstSqBB;
}

constexpr Bitboard setOccupancy(uint64_t index, Bitboard movementMask) {
    Bitboard occupancy = 0ull;
    int count = 0;

    while (movementMask) {
        int square = __builtin_ctzll(movementMask);

        if (index & (1 << count)) {
            occupancy |= (1ull << square);
        }

        count++;
        movementMask &= movementMask - 1;  // pop LSB
    }

    return occupancy;
}

constexpr void init() {
    // bishop
    for (Square sq : ALL_SQUARES) {
        const Bitboard bishopMoveMask = computeBishopMovementMask(sq);

        // write mask to array
        BISHOP_MOVEMENT_MASKS[sq] = bishopMoveMask;

        // iterate over all 2^n variations, where n is the number of set 1s in this mask
        const uint64_t occupancyVariations = 1 << __builtin_popcountll(bishopMoveMask);
        for (uint64_t index = 0; index < occupancyVariations; index++) {
            // initialize attacks for this square at this magic index
            const Bitboard occupancy = setOccupancy(index, bishopMoveMask);
            const uint64_t relevantMagic = BISHOP_MAGICS[sq] >> (64 - BISHOP_RELEVANT_BITS[sq]);
            const uint64_t magicIdx = occupancy * relevantMagic;
            BISHOP_ATTACKS[sq][magicIdx] = computeBishopMovementMask(sq, occupancy);
        }
    }

    // rook (same steps)
    for (Square sq : ALL_SQUARES) {
        const Bitboard rookMoveMask = computeRookMovementMask(sq);
        ROOK_MOVEMENT_MASKS[sq] = rookMoveMask;

        const uint64_t occupancyVariations = 1 << __builtin_popcountll(rookMoveMask);
        for (uint64_t index = 0; index < occupancyVariations; index++) {
            const Bitboard occupancy = setOccupancy(index, rookMoveMask);
            const uint64_t relevantMagic = ROOK_MAGICS[sq] >> (64 - ROOK_RELEVANT_BITS[sq]);
            const uint64_t magicIdx = occupancy * relevantMagic;
            ROOK_ATTACKS[sq][magicIdx] = computeRookMovementMask(sq, occupancy);
        }
    }
}

};  // namespace Magic
