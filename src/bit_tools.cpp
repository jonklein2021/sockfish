#include "bit_tools.h"

#include "types.h"

#include <iostream>
#include <sstream>

void prettyPrintPosition(const Bitboard pieceBits[12], const bool noFlip) {
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

void printBitboard(const Bitboard bitboard) {
    std::ostringstream out;
    for (int y = 0; y < 8; y++) {
        out << (8 - y) << "  ";
        for (int x = 0; x < 8; x++) {
            out << ((bitboard & coordsToBit(x, y)) ? "1" : "0") << " ";
        }
        out << "\n";
    }

    out << "\n   a b c d e f g h\n";
    std::cout << out.str() << std::endl;
}

void printBoards(const Bitboard pieceBits[12]) {
    for (int i = 0; i < 12; i++) {
        std::cout << pieceFilenames[i] << std::endl;
        printBitboard(pieceBits[i]);
    }
}
