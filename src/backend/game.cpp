#include "constants.h"
#include "game.h"

void printU64(uint64_t n) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::cout << (n & (1ull << (i * 8 + j)) ? "1" : "0") << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void print(const BitBoard &board, PieceType p) {
    printU64(board.pieceBits[p]);
}

void print(const BitBoard &board) {
    for (int i = 0; i < 12; i++) {
        std::cout << pieceNames[i] << std::endl;
        print(board, static_cast<PieceType>(i));
    }
}

std::string to_string(const Move &move) {
    std::ostringstream ss;
    
    ss << pieceNames[move.pieceMoved] << ": (" 
    << move.from.x << ", " << move.from.y << ") -> ("
    << move.to.x << ", " << move.to.y << ")";

    return ss.str();
}

void prettyPrint(const BitBoard &board) {
    std::string out;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bool found = false;
            for (int p = 0; p < 12; p++) {
                if (board.pieceBits[p] & (1ull << (i * 8 + j))) {
                    out += pieceNames[p];
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

    std::cout << out << std::endl;
}

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
            uint64_t* piece = nullptr;
            switch (c) {
                case 'P': piece = &board.pieceBits[WP]; break;
                case 'N': piece = &board.pieceBits[WN]; break;
                case 'B': piece = &board.pieceBits[WB]; break;
                case 'R': piece = &board.pieceBits[WR]; break;
                case 'Q': piece = &board.pieceBits[WQ]; break;
                case 'K': piece = &board.pieceBits[WK]; break;
                case 'p': piece = &board.pieceBits[BP]; break;
                case 'n': piece = &board.pieceBits[BN]; break;
                case 'b': piece = &board.pieceBits[BB]; break;
                case 'r': piece = &board.pieceBits[BR]; break;
                case 'q': piece = &board.pieceBits[BQ]; break;
                case 'k': piece = &board.pieceBits[BK]; break;
            }

            // set this piece's bit at the correct position
            *piece |= 1ull << (y * 8 + x);
            x++;
        }
    }
    return board;
}

std::string bitBoardToFen(const BitBoard& board) {
    return "";
}

void GameState::ApplyMove(const Move &move) {
    whiteToMove = !whiteToMove;
    BitBoard *bitBoard = &this->board;
    
    // useful 64-bit words
    uint64_t from = 1ull << (move.from.y * 8 + move.from.x);
    uint64_t to = 1ull << (move.to.y * 8 + move.to.x);
    uint64_t fromTo = from | to;

    /* CASTLING */
    
    // white king side castle
    if (move.pieceMoved == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 6 && move.to.y == 7) {
        board.pieceBits[WK] ^= fromTo;
        board.pieceBits[WR] ^= (1ull << 63) | (1ull << 61);
        whiteKingMoved = true;
        return;
    }
    
    // white queen side castle
    if (move.pieceMoved == WK && move.from.x == 4 && move.from.y == 7 && move.to.x == 2 && move.to.y == 7) {
        board.pieceBits[WK] ^= fromTo;
        board.pieceBits[WR] ^= (1ull << 56) | (1ull << 59);
        whiteKingMoved = true;
        return;
    }

    // black king side castle
    if (move.pieceMoved == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 6 && move.to.y == 0) {
        board.pieceBits[BK] ^= fromTo;
        board.pieceBits[BR] ^= (1ull << 7) | (1ull << 5);
        blackKingMoved = true;
        return;
    }

    // black queen side castle
    if (move.pieceMoved == BK && move.from.x == 4 && move.from.y == 0 && move.to.x == 2 && move.to.y == 0) {
        board.pieceBits[BK] ^= fromTo;
        board.pieceBits[BR] ^= (1ull << 0) | (1ull << 3);
        blackKingMoved = true;
        return;
    }
    
    /* BASIC MOVES AND CAPTURES */
    
    // simultaneously zero out the bit of the piece's old position
    // AND mark the bit of the piece's new position using xor
    board.pieceBits[move.pieceMoved] ^= fromTo;
    
    // determine what piece to remove if there is a capture
    if (move.isCapture) {
        for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
            if (p != move.pieceMoved && board.pieceBits[p] & to) {
                // zero out the captured piece's bit
                board.pieceBits[p] &= ~to;
                break;
            }
        }
    }
    
    // handle moves that revoke castling rights
    if (move.pieceMoved == WK) whiteKingMoved = true;
    if (move.pieceMoved == BK) blackKingMoved = true;
    if (move.pieceMoved == WR && move.from.x == 0) whiteRookAMoved = true;
    if (move.pieceMoved == WR && move.from.x == 7) whiteRookHMoved = true;
    if (move.pieceMoved == BR && move.from.x == 0) blackRookAMoved = true;
    if (move.pieceMoved == BR && move.from.x == 7) blackRookHMoved = true;

}

bool GameState::IsCheck() {
    return false;
}

std::vector<Move> generateMoves(const GameState& state) {
    std::vector<Move> moves; // TODO: Check if a set would be a better choice
    bool white = state.whiteToMove;

    // get all pieces
    uint64_t pawns = white ? state.board.pieceBits[WP] : state.board.pieceBits[BP];
    uint64_t knights = white ? state.board.pieceBits[WN] : state.board.pieceBits[BN];
    uint64_t bishops = white ? state.board.pieceBits[WB] : state.board.pieceBits[BB];
    uint64_t rooks = white ? state.board.pieceBits[WR] : state.board.pieceBits[BR];
    uint64_t queens = white ? state.board.pieceBits[WQ] : state.board.pieceBits[BQ];
    uint64_t king = white ? state.board.pieceBits[WK] : state.board.pieceBits[BK];
    
    uint64_t oppPawns = white ? state.board.pieceBits[BP] : state.board.pieceBits[WP];
    uint64_t oppKnights = white ? state.board.pieceBits[BN] : state.board.pieceBits[WN];
    uint64_t oppBishops = white ? state.board.pieceBits[BB] : state.board.pieceBits[WB];
    uint64_t oppRooks = white ? state.board.pieceBits[BR] : state.board.pieceBits[WR];
    uint64_t oppQueens = white ? state.board.pieceBits[BQ] : state.board.pieceBits[WQ];
    uint64_t oppKing = white ? state.board.pieceBits[BK] : state.board.pieceBits[WK];
    
    // useful for determining captures
    uint64_t myPieces = pawns | knights | bishops | rooks | queens | king;
    uint64_t oppPieces = oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
    uint64_t allPieces = myPieces | oppPieces;
    uint64_t emptySquares = ~allPieces;

    printU64(emptySquares);

    // initial and final positions of each piece
    int xi, yi, xf, yf;

    /* PAWN MOVES */
    while (pawns > 0) {
        int trailingZeros = __builtin_ctzll(pawns);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        // generate all possible pawn moves
        int direction = white ? -1 : 1;
        xf = xi;
        yf = yi + direction;

        // move one square forward
        uint64_t to = 1ull << (yf * 8 + xf);
        if (yf >= 0 && yf < 8) {
            if (to & emptySquares) {
                moves.push_back({{xi, yi}, {xf, yf}, (white ? WP : BP), false});
            }
        }

        // move two squares forward only if pawn is in its starting position
        if ((white && yi == 6) || (!white && yi == 1)) {
            uint64_t over = 1ull << ((yi + direction) * 8 + xf); // square pawn jumps over
            uint64_t to = 1ull << (yf * 8 + xf); // destination square
            
            // both `over` and `to` must be empty for a pawn to move 2 squares
            if ((emptySquares & (over | to)) == (over | to)) {
                moves.push_back({{xi, yi}, {xf, yf + direction}, (white ? WP : BP), false});
            }
        }
        

        // capture moves
        std::vector<std::pair<int, int>> pawnCaptures = {{1, direction}, {-1, direction}};
        for (auto &[dx, dy] : pawnCaptures) {
            xf = xi + dx;
            yf = yi + dy;
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                to = 1ull << (yf * 8 + xf);
                if (to & oppPieces) {
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WP : BP), true});
                }
            }
        }

        // TODO: en passant and promotion

        pawns &= pawns - 1;
    }

    /* KNIGHT MOVES */
    while (knights > 0) {
        int trailingZeros = __builtin_ctzll(knights);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        // generate all possible knight moves
        std::vector<std::pair<int, int>> knightMoves = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
        for (auto &[dx, dy] : knightMoves) {
            xf = xi + dx;
            yf = yi + dy;
            
            // ensure this move is within bounds
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                // bit of the destination square
                uint64_t to = 1ull << (yf * 8 + xf);

                if (to & oppPieces) {
                    // move is a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WN : BN), true});
                } else if (to & emptySquares) { // necessary so we don't move to our own piece
                    // move is not a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WN : BN), false});
                }
            }
        }

        // clear this knight from the bitboard
        knights &= knights - 1;
    }

    /* BISHOP MOVES */
    while (bishops > 0) {
        int trailingZeros = __builtin_ctzll(bishops);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        // generate all possible bishop moves
        std::vector<std::pair<int, int>> bishopMoves = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (auto &[dx, dy] : bishopMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // while move is in bounds, move in that direction
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                if (to & oppPieces) {
                    // move is a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WB : BB), true});
                    break;
                } else if (to & emptySquares) {
                    // move is not a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WB : BB), false});
                } else {
                    break;
                }
                xf += dx;
                yf += dy;
            }
        }

        bishops &= bishops - 1;
    }

    /* ROOK MOVES */
    while (rooks > 0) {
        int trailingZeros = __builtin_ctzll(rooks);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        // generate all possible rook moves
        std::vector<std::pair<int, int>> rookMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (auto &[dx, dy] : rookMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // while move is in bounds, move in that direction
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                if (to & oppPieces) {
                    // move is a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WR : BR), true});
                    break;
                } else if (to & emptySquares) {
                    // move is not a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WR : BR), false});
                } else {
                    break;
                }
                xf += dx;
                yf += dy;
            }
        }

        // clear this rook from the bitboard
        rooks &= rooks - 1;
    }

    /* QUEEN MOVES */
    while (queens > 0) {
        int trailingZeros = __builtin_ctzll(queens);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        // generate all possible queen moves
        std::vector<std::pair<int, int>> queenMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (auto &[dx, dy] : queenMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // while move is in bounds, move in that direction
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                if (to & oppPieces) {
                    // move is a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WQ : BQ), true});
                    break;
                } else if (to & emptySquares) {
                    // move is not a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WQ : BQ), false});
                } else {
                    break;
                }
                xf += dx;
                yf += dy;
            }
        }

        // clear this queen from the bitboard
        queens &= queens - 1;
    }

    /* KING MOVES */
    while (king > 0) {
        int trailingZeros = __builtin_ctzll(king);
        xi = trailingZeros % 8;
        yi = trailingZeros / 8;

        // generate all possible king moves
        std::vector<std::pair<int, int>> kingMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (auto &[dx, dy] : kingMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // ensure this move is within bounds
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                if (to & oppPieces) {
                    // move is a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WK : BK), true});
                } else if (to & emptySquares) {
                    // move is not a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WK : BK), false});
                }
            }
        }

        // see if king can castle
        // TODO: test for check violations
        if (white) {
            // kingside castle
            if (!state.whiteKingMoved && !state.whiteRookAMoved) {
                if ((emptySquares & (1ull << 61)) && (emptySquares & (1ull << 62))) {
                    moves.push_back({{4, 7}, {6, 7}, WK, false});
                }
            }

            // queenside castle
            if (!state.whiteKingMoved && !state.whiteRookHMoved) {
                if ((emptySquares & (1ull << 57)) && (emptySquares & (1ull << 58)) && (emptySquares & (1ull << 59))) {
                    moves.push_back({{4, 7}, {2, 7}, WK, false});
                }
            }
        } else {
            // kingside castle
            if (!state.blackKingMoved && !state.blackRookHMoved) {
                if ((emptySquares& (1ull << 5)) && (emptySquares& (1ull << 6))) {
                    moves.push_back({{4, 0}, {6, 0}, BK, false});
                }
            }
            
            // queenside castle
            if (!state.blackKingMoved && !state.blackRookAMoved) {
                if ((emptySquares& (1ull << 1)) && (emptySquares& (1ull << 2)) && (emptySquares& (1ull << 3))) {
                    moves.push_back({{4, 0}, {2, 0}, BK, false});
                }
            }
        }

        // clear this king from the bitboard
        king &= king - 1;
    }

    
    return moves;
}

/**
 * N.B: This method IGNORES the isCapture
 * member for convenience
 * 
 * @param other move to compare to
 */
bool Move::equals(const Move &other) {
    return (
        pieceMoved == other.pieceMoved &&
        from.x == other.from.x &&
        from.y == other.from.y &&
        to.x == other.to.x &&
        to.y == other.to.y
    );
}
