#include <iostream>

#include "BitBoard.h"
#include "constants.h"

BitBoard fenToBitBoard(const std::string& fen) {
    BitBoard board;
    int x = 0, y = 0;
    for (const char& c : fen) {
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square; skip x squares
            x += c - '0';
        } else { // piece
            // set this piece's bit at the correct position
            board.pieceBits[fenPieceMap[c]] |= 1ull << (y * 8 + x);
            x++;
        }
    }
    return board;
}

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

void BitBoard::applyMove(const Move& move) {    
    // useful 64-bit words
    uint64_t from = 1ull << (move.from.y * 8 + move.from.x);
    uint64_t to = 1ull << (move.to.y * 8 + move.to.x);
    uint64_t fromTo = from | to;

    /* CASTLING */
    
    // white king side castle
    if (move.pieceMoved == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 6 && move.to.y == 7) {
        pieceBits[WK] ^= fromTo;
        pieceBits[WR] ^= (1ull << 63) | (1ull << 61);
        return;
    }
    
    // white queen side castle
    if (move.pieceMoved == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 2 && move.to.y == 7) {
        pieceBits[WK] ^= fromTo;
        pieceBits[WR] ^= (1ull << 56) | (1ull << 59);
        return;
    }

    // black king side castle
    if (move.pieceMoved == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 6 && move.to.y == 0) {
        pieceBits[BK] ^= fromTo;
        pieceBits[BR] ^= (1ull << 7) | (1ull << 5);
        return;
    }

    // black queen side castle
    if (move.pieceMoved == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 2 && move.to.y == 0) {
        pieceBits[BK] ^= fromTo;
        pieceBits[BR] ^= (1ull << 0) | (1ull << 3);
        return;
    }
    
    /* BASIC MOVES AND CAPTURES */
    
    // "moves" the bit of the piece's old location to its new location
    pieceBits[move.pieceMoved] ^= fromTo;
    
    // determine what piece to remove if there is a capture
    if (move.isCapture) {
        for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
            if (p != move.pieceMoved && pieceBits[p] & to) {
                // zero out the captured piece's bit
                pieceBits[p] &= ~to;
                break;
            }
        }
    }

    /* PAWN PROMOTION */
    if (move.promotionPiece != None) {
        pieceBits[move.pieceMoved] &= ~to; // remove pawn
        pieceBits[move.promotionPiece] |= to; // add promoted piece
    }

    /* EN PASSANT */

}

std::string BitBoard::to_string() {
    std::string out = "    0  1  2  3  4  5  6  7\n";

    for (int i = 0; i < 8; i++) {
        out += " " + std::to_string(i) + " ";
        for (int j = 0; j < 8; j++) {
            bool found = false;
            for (int p = 0; p < 12; p++) {
                if (pieceBits[p] & (1ull << (i * 8 + j))) {
                    out += pieceFilenames[p];
                    found = true;
                    break;
                }
            }
            if (!found) {
                out += "--";
            }
            out += " ";
        }
        out += "\n";
    }

    return out;
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
