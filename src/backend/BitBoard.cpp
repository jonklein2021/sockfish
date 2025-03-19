#include <iostream>

#include "BitBoard.h"
#include "constants.h"

// TODO: Make this more efficient by backward-searching from the sqaure of interest
bool BitBoard::attacked(sf::Vector2<int> square, bool white) const {
    // std::cout << "Checking if (" << square.x << ", " << square.y << ") is under attack by " << (white ? "white" : "black") << std::endl;
    uint64_t targetBit = 1ull << (square.y * 8 + square.x);

    // get all pieces
    uint64_t oppPawns = pieceBits[white ? WP : BP];
    uint64_t oppKnights = pieceBits[white ? WN : BN];
    uint64_t oppBishops = pieceBits[white ? WB : BB];
    uint64_t oppRooks = pieceBits[white ? WR : BR];
    uint64_t oppQueens = pieceBits[white ? WQ : BQ];
    uint64_t oppKing = pieceBits[white ? WK : BK];
    
    uint64_t pawns = pieceBits[white ? BP : WP];
    uint64_t knights = pieceBits[white ? BN : WN];
    uint64_t bishops = pieceBits[white ? BB : WB];
    uint64_t rooks = pieceBits[white ? BR : WR];
    uint64_t queens = pieceBits[white ? BQ : WQ];
    uint64_t king = pieceBits[white ? BK : WK];
    
    // useful for determining captures
    uint64_t myPieces = pawns | knights | bishops | rooks | queens | king;
    uint64_t oppPieces = oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;

    int xi, yi, xf, yf;
    
    // pawn test
    int direction = white ? -1 : 1;
    while (oppPawns > 0) {
        int trailingZeros = __builtin_ctzll(oppPawns);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        for (auto &[dx, dy] : pawnCaptures) {
            xf = xi + dx;
            yf = yi + dy * direction;
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                if (to & targetBit) {
                    // std::cout << "Pawn capture: (" << xi << ", " << yi << ") -> (" << xf << ", " << yf << ")" << std::endl;
                    return true;
                }
            }
        }

        // TODO: en passant

        oppPawns &= oppPawns - 1;
    }

    // knight test
    while (oppKnights > 0) {
        int trailingZeros = __builtin_ctzll(oppKnights);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        for (auto &[dx, dy] : knightMoves) {
            xf = xi + dx;
            yf = yi + dy;
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                if (to & targetBit) {
                    // std::cout << "Knight capture: (" << xi << ", " << yi << ") -> (" << xf << ", " << yf << ")" << std::endl;
                    return true;
                }
            }
        }

        oppKnights &= oppKnights - 1;
    }

    // bishop test
    while (oppBishops > 0) {
        int trailingZeros = __builtin_ctzll(oppBishops);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        for (auto &[dx, dy] : bishopMoves) {
            xf = xi + dx;
            yf = yi + dy;
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);

                if (to & targetBit) {
                    // std::cout << "Bishop capture: (" << xi << ", " << yi << ") -> (" << xf << ", " << yf << ")" << std::endl;
                    return true;
                }

                // stop if there’s a blocking piece
                if ((to & oppPieces) || (to & myPieces)) {
                    break;
                }

                xf += dx;
                yf += dy;
            }
        }

        oppBishops &= oppBishops - 1;
    }

    // rook test
    while (oppRooks > 0) {
        int trailingZeros = __builtin_ctzll(oppRooks);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;
        for (auto &[dx, dy] : rookMoves) {
            xf = xi + dx;
            yf = yi + dy;
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);

                
                if (to & targetBit) {
                    // std::cout << "Rook capture: (" << xi << ", " << yi << ") -> (" << xf << ", " << yf << ")" << std::endl;
                    return true;
                }
                
                // stop if there’s a blocking piece
                if ((to & oppPieces) || (to & myPieces)) {
                    break;
                }

                xf += dx;
                yf += dy;
            }
        }
        oppRooks &= oppRooks - 1;
    }

    // queen test
    while (oppQueens > 0) {
        int trailingZeros = __builtin_ctzll(oppQueens);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;
        for (auto &[dx, dy] : queenMoves) {
            xf = xi + dx;
            yf = yi + dy;
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                
                if (to & targetBit) {
                    // std::cout << "Queen capture: (" << xi << ", " << yi << ") -> (" << xf << ", " << yf << ")" << std::endl;
                    return true;
                }

                // stop if there’s a blocking piece
                if (to & oppPieces || to & myPieces) {
                    break;
                }

                xf += dx;
                yf += dy;
            }
        }
        oppQueens &= oppQueens - 1;
    }

    // king test
    int trailingZeros = __builtin_ctzll(oppKing);
    xi = trailingZeros % 8;
    yi = trailingZeros / 8;
    for (auto &[dx, dy] : kingMoves) {
        xf = xi + dx;
        yf = yi + dy;
        if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
            uint64_t to = 1ull << (yf * 8 + xf);
            if (to & targetBit) {
                // std::cout << "King capture: (" << xi << ", " << yi << ") -> (" << xf << ", " << yf << ")" << std::endl;
                return true;
            }
        }
    }

    return false;
}

void BitBoard::makeMove(const Move& move) {    
    // useful 64-bit words
    uint64_t from = 1ull << (move.from.y * 8 + move.from.x);
    uint64_t to = 1ull << (move.to.y * 8 + move.to.x);
    uint64_t fromTo = from | to;

    /* CASTLING */
    
    // white king side castle
    if (move.piece == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 6 && move.to.y == 7) {
        pieceBits[WK] ^= fromTo;
        pieceBits[WR] ^= (1ull << 63) | (1ull << 61);
        return;
    }
    
    // white queen side castle
    if (move.piece == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 2 && move.to.y == 7) {
        pieceBits[WK] ^= fromTo;
        pieceBits[WR] ^= (1ull << 56) | (1ull << 59);
        return;
    }

    // black king side castle
    if (move.piece == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 6 && move.to.y == 0) {
        pieceBits[BK] ^= fromTo;
        pieceBits[BR] ^= (1ull << 7) | (1ull << 5);
        return;
    }

    // black queen side castle
    if (move.piece == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 2 && move.to.y == 0) {
        pieceBits[BK] ^= fromTo;
        pieceBits[BR] ^= (1ull << 0) | (1ull << 3);
        return;
    }
    
    /* BASIC MOVES */
    
    // "move" the bit of the piece's old location to its new location
    pieceBits[move.piece] ^= fromTo;
    
    /* CAPTURES */

    // determine what piece to remove if there is a capture
    if (move.isCapture) {
        if (move.isEnPassant) {
            // the piece to remove is at the attacker pawn's new x and old y
            uint64_t removeBit = (1ull << (move.from.y * 8 + move.to.x));
            
            // remove the captured pawn
            pieceBits[move.piece == WP ? BP : WP] &= ~removeBit;
        } else {
            // remove the captured piece
            for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
                if (p != move.piece && (pieceBits[p] & to)) {
                    // zero out the captured piece's bit
                    pieceBits[p] &= ~to;
                    break;
                }
            }
        }
    }

    /* PAWN PROMOTION */
    if (move.promotionPiece != None) {
        pieceBits[move.piece] &= ~to; // remove pawn
        pieceBits[move.promotionPiece] |= to; // add promoted piece
    }
}

void BitBoard::unmakeMove(const Move &move, const Metadata &metadata) {
    uint64_t from = 1ull << (move.from.y * 8 + move.from.x);
    uint64_t to = 1ull << (move.to.y * 8 + move.to.x);
    uint64_t fromTo = from | to;

    /* CASTLING */
    
    // white king-side castle
    if (move.piece == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 6 && move.to.y == 7) {
        pieceBits[WK] ^= fromTo;
        pieceBits[WR] ^= (1ull << 63) | (1ull << 61);
        return;
    }
    
    // white queen-side castle
    if (move.piece == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 2 && move.to.y == 7) {
        pieceBits[WK] ^= fromTo;
        pieceBits[WR] ^= (1ull << 56) | (1ull << 59);
        return;
    }

    // black king-side castle
    if (move.piece == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 6 && move.to.y == 0) {
        pieceBits[BK] ^= fromTo;
        pieceBits[BR] ^= (1ull << 7) | (1ull << 5);
        return;
    }

    // black queen-side castle
    if (move.piece == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 2 && move.to.y == 0) {
        pieceBits[BK] ^= fromTo;
        pieceBits[BR] ^= (1ull << 0) | (1ull << 3);
        return;
    }
    
    /* BASIC MOVES */
    
    // "Move" the bit of the piece back to its original position
    pieceBits[move.piece] ^= fromTo;

    /* CAPTURES */
    if (move.isCapture) {
        if (move.isEnPassant) {
            uint64_t restoreBit = (1ull << (move.from.y * 8 + move.to.x));
            pieceBits[move.piece == WP ? BP : WP] |= restoreBit;
        } else {
            pieceBits[metadata.capturedPiece] |= to;  // Restore captured piece
        }
    }

    /* PAWN PROMOTION */
    if (move.promotionPiece != None) {
        pieceBits[move.piece] |= from;  // Restore original pawn
        pieceBits[move.promotionPiece] &= ~to;  // Remove promoted piece
    }
}

bool BitBoard::isDraw() const {
    return pieceBits[WP] == 0 &&
           pieceBits[WN] == 0 &&
           pieceBits[WB] == 0 &&
           pieceBits[WR] == 0 &&
           pieceBits[WQ] == 0 &&
           pieceBits[BP] == 0 &&
           pieceBits[BN] == 0 &&
           pieceBits[BB] == 0 &&
           pieceBits[BR] == 0 &&
           pieceBits[BQ] == 0;
};

PieceType BitBoard::getPieceType(sf::Vector2<int> square) const {
    uint64_t targetBit = 1ull << (square.y * 8 + square.x);
    for (int i = 0; i < 12; i++) {
        if (pieceBits[i] & targetBit) {
            return static_cast<PieceType>(i);
        }
    }
    return None;
}

void BitBoard::prettyPrint(bool noFlip) {
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

void printU64(uint64_t n) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::cout << (n & (1ull << (i * 8 + j)) ? "1" : "0") << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void BitBoard::print(PieceType p) {
    printU64(pieceBits[p]);
}

void BitBoard::print() {
    for (int i = 0; i < 12; i++) {
        std::cout << pieceFilenames[i] << std::endl;
        print(static_cast<PieceType>(i));
    }
}
