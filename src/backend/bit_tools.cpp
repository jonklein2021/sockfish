#include "bit_tools.h"

uint64_t getBit(uint64_t bitboard, int index) {
    return bitboard & (1ull << index);
}

void setBit(uint64_t& bitboard, int index) {
    bitboard |= (1ull << index);
}

void popBit(uint64_t& bitboard, int index) {
    if (getBit(bitboard, index)) {
        bitboard ^= (1ull << index);
    }
}

uint64_t coordsToBit(int x, int y) {
    return (1ull << (y * 8 + x));
}

uint64_t coordsToBit(sf::Vector2<int> coords) {
    return coordsToBit(coords.x, coords.y);
}

sf::Vector2<int> bitToCoords(uint64_t bit) {
    int index = __builtin_ctzll(bit);
    return {index % 8, index / 8};
}

/*** MOVE GENERATION TOOLS ***/

uint64_t computePawnAttacks(const uint64_t squareBit, const bool white) {
    if (white) {
        return ((squareBit & not_file_h) >> 7) | ((squareBit & not_file_a) >> 9); // up right, up left
    } else {
        return ((squareBit & not_file_a) << 7) | ((squareBit & not_file_h) << 9); // down left, down right
    }
}

uint64_t computeKnightAttacks(const uint64_t squareBit) {
    const uint64_t ddl = (squareBit & not_rank_12 & not_file_a) << 15; // down 2, left 1
    const uint64_t ddr = (squareBit & not_rank_12 & not_file_h) << 17; // down 2, right 1
    const uint64_t drr = (squareBit & not_rank_1 & not_file_gh) << 10; // down 1, right 2
    const uint64_t dll = (squareBit & not_rank_1 & not_file_ab) << 6; // down 1, left 2
    const uint64_t uur = (squareBit & not_rank_78 & not_file_h) >> 15; // up 2, right 1
    const uint64_t uul = (squareBit & not_rank_78 & not_file_a) >> 17; // up 2, left 1
    const uint64_t ull = (squareBit & not_rank_8 & not_file_ab) >> 10; // up 1, left 2
    const uint64_t urr = (squareBit & not_rank_8 & not_file_gh) >> 6; //  up 1, right 2
    
    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

uint64_t computeBishopAttacks(const uint64_t squareBit) {
    uint64_t attacks = 0;
    uint64_t mask = squareBit;
    uint64_t filteredMask;
    
    // up right
    while (filteredMask = (mask & not_rank_8 & not_file_h)) {
        mask = filteredMask >> 7;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // up left
    while (filteredMask = (mask & not_rank_8 & not_file_a)) {
        mask = filteredMask >> 9;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // down right
    while (filteredMask = (mask & not_rank_1 & not_file_h)) {
        mask = filteredMask << 9;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // down left
    while (filteredMask = (mask & not_rank_1 & not_file_a)) {
        mask = filteredMask << 7;
        attacks |= mask;
    }
    
    return attacks;
}

uint64_t computeRookAttacks(const uint64_t squareBit) {
    uint64_t attacks = 0;
    uint64_t mask = squareBit;
    uint64_t filteredMask;
    
    // up
    while (filteredMask = (mask & not_rank_8)) {
        mask = filteredMask >> 8;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // down
    while (filteredMask = (mask & not_rank_1)) {
        mask = filteredMask << 8;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // left
    while (filteredMask = (mask & not_file_a)) {
        mask = filteredMask >> 1;
        attacks |= mask;
    }
    
    mask = squareBit;
    
    // right
    while (filteredMask = (mask & not_file_h)) {
        mask = filteredMask << 1;
        attacks |= mask;
    }
    
    return attacks;
}

uint64_t computeQueenAttacks(const uint64_t squareBit) {
    return computeBishopAttacks(squareBit) | computeRookAttacks(squareBit); // heheh
}

uint64_t computeKingAttacks(const uint64_t squareBit) {
    const uint64_t d = (squareBit & not_rank_1) << 8; // down
    const uint64_t u = (squareBit & not_rank_8) >> 8; // up
    const uint64_t l = (squareBit & not_file_a) >> 1; // left
    const uint64_t r = (squareBit & not_file_h) << 1; // right
    const uint64_t dl = (squareBit & not_rank_1 & not_file_a) << 7; // down left
    const uint64_t dr = (squareBit & not_rank_1 & not_file_h) << 9; // down right
    const uint64_t ul = (squareBit & not_rank_8 & not_file_a) >> 9; // up left
    const uint64_t ur = (squareBit & not_rank_8 & not_file_h) >> 7; // up right
    
    return d | u | l | r | dl | dr | ul | ur;
}

void prettyPrintPosition(const uint64_t pieceBits[12], const bool noFlip) {
    std::ostringstream out;
    out << "\n";

    for (int i = 0; i < 8; i++) {
        int rank = noFlip ? i : 7 - i;
        
        // row border
        out << "   +----+----+----+----+----+----+----+----+\n";

        // rank number
        out << " " << (8 - rank) << " ";

        for (int j = 0; j < 8; j++) {
            int file = noFlip ? j : 7 - j;
            int squareIndex = rank * 8 + file;
            bool found = false;

            out << "| ";

            for (int p = 0; p < 12; p++) {
                if (pieceBits[p] & (1ull << squareIndex)) {
                    out << pieceFilenames[p] << " ";
                    found = true;
                    break;
                }
            }

            if (!found) {
                out << "   ";
            }
        }

        out << "|\n";
    }

    // bottom border
    out << "   +----+----+----+----+----+----+----+----+\n";

    // file indices
    out << "     ";
    if (noFlip) {
        out << "a    b    c    d    e    f    g    h  \n";
    } else {
        out << "h    g    f    e    d    c    b    a  \n";
    }

    std::cout << out.str() << std::endl;
}

void printU64(const uint64_t bitboard) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            std::cout << ((bitboard & coordsToBit(x, y)) ? "1" : "0") << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printBoards(const uint64_t pieceBits[12]) {
    for (int i = 0; i < 12; i++) {
        std::cout << pieceFilenames[i] << std::endl;
        printU64(pieceBits[i]);
    }
}
