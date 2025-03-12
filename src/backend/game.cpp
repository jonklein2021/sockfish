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
    std::string out = "      0  1  2  3  4  5  6  7\n";
    out += "    ------------------------\n";

    for (int i = 0; i < 8; i++) {
        out += " " + std::to_string(i) + " | ";
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
            switch (c) { // TODO: use fenToPiece map instead of this
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

bool BitBoard::attacked(sf::Vector2<int> square, bool white) const {
    std::cout << "Checking if (" << square.x << ", " << square.y << ") is under attack by " << (white ? "white" : "black") << std::endl;
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
    uint64_t allPieces = myPieces | oppPieces;
    uint64_t emptySquares = ~allPieces;

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
                    std::cout << "Pawn capture" << std::endl;
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
                    std::cout << "Knight capture" << std::endl;
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
                    std::cout << "Bishop capture" << std::endl;
                    return true;
                } else if (to & oppPieces) {
                    // can't capture through pieces
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
                    std::cout << "Rook capture" << std::endl;
                    return true;
                } else if (to & oppPieces) {
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
                    std::cout << "Queen capture" << std::endl;
                    return true;
                } else if (to & oppQueens) {
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
            if (to & oppKing) {
                std::cout << "King capture" << std::endl;
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
}

void GameState::applyMove(const Move &move) {
    // make changes to board representation
    board.applyMove(move);

    // make state changes
    whiteToMove = !whiteToMove;
    
    // rook moved or captured
    if (move.to.x == 0 && move.to.y == 0) blackRookAMoved = true;
    if (move.to.x == 7 && move.to.y == 0) blackRookHMoved = true;
    if (move.to.x == 0 && move.to.y == 7) whiteRookAMoved = true;
    if (move.to.x == 7 && move.to.y == 7) whiteRookHMoved = true;
    if (move.pieceMoved == BR && move.from.x == 0) blackRookAMoved = true;
    if (move.pieceMoved == BR && move.from.x == 7) blackRookHMoved = true;
    if (move.pieceMoved == WR && move.from.x == 0) whiteRookAMoved = true;
    if (move.pieceMoved == WR && move.from.x == 7) whiteRookHMoved = true;

    // king moved
    if (move.pieceMoved == WK) whiteKingMoved = true;
    if (move.pieceMoved == BK) blackKingMoved = true;
}

bool GameState::underAttack(sf::Vector2<int> square) const {
    return board.attacked(square, !whiteToMove);
}

bool GameState::isCheck() const {
    // get position of king
    uint64_t king = board.pieceBits[whiteToMove ? WK : BK];
    int trailingZeros = __builtin_ctzll(king);
    int x = trailingZeros % 8, y = trailingZeros / 8;

    // check if king is under attack
    return underAttack({x, y});
}

// Can this be in the BitBoard class?
std::vector<Move> GameState::generateMoves() const {
    std::vector<Move> moves; // TODO: Check if a set would be a better choice
    const bool white = whiteToMove;
    const uint64_t *pieceBits = board.pieceBits;

    // get all pieces
    uint64_t pawns = pieceBits[white ? WP : BP];
    uint64_t knights = pieceBits[white ? WN : BN];
    uint64_t bishops = pieceBits[white ? WB : BB];
    uint64_t rooks = pieceBits[white ? WR : BR];
    uint64_t queens = pieceBits[white ? WQ : BQ];
    uint64_t king = pieceBits[white ? WK : BK];
    
    uint64_t oppPawns = pieceBits[white ? BP : WP];
    uint64_t oppKnights = pieceBits[white ? BN : WN];
    uint64_t oppBishops = pieceBits[white ? BB : WB];
    uint64_t oppRooks = pieceBits[white ? BR : WR];
    uint64_t oppQueens = pieceBits[white ? BQ : WQ];
    uint64_t oppKing = pieceBits[white ? BK : WK];
    
    // useful for determining captures
    uint64_t myPieces = pawns | knights | bishops | rooks | queens | king;
    uint64_t oppPieces = oppPawns | oppKnights | oppBishops | oppRooks | oppQueens | oppKing;
    uint64_t allPieces = myPieces | oppPieces;
    uint64_t emptySquares = ~allPieces;

    // initial and final positions of each piece
    int xi, yi, xf, yf;

    // get position of king
    sf::Vector2<int> kingPos{__builtin_ctzll(king) % 8, __builtin_ctzll(king) / 8};

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
                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP), false};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudolegal);

                // add move if king is not under attack by opponent in resulting position
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
                }
            }
        }

        // move two squares forward only if pawn is in its starting position
        if ((white && yi == 6) || (!white && yi == 1)) {
            uint64_t over = 1ull << ((yi + direction) * 8 + xf); // square pawn jumps over
            uint64_t to = 1ull << (yf * 8 + xf); // destination square
            yf += direction;
            
            // both `over` and `to` must be empty for a pawn to move 2 squares
            if ((emptySquares & (over | to)) == (over | to)) {
                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP), false};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudolegal);

                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
                }
            }
        }

        // pawn capture moves
        for (auto &[dx, dy] : pawnCaptures) {
            xf = xi + dx;
            yf = yi + dy * direction;
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                to = 1ull << (yf * 8 + xf);
                if (to & oppPieces) {
                    Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP), true};
                    BitBoard tempBoard(board);
                    tempBoard.applyMove(pseudolegal);
                    
                    if (!tempBoard.attacked(kingPos, !white)) {
                        moves.push_back(pseudolegal);
                    }
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
        for (auto &[dx, dy] : knightMoves) {
            xf = xi + dx;
            yf = yi + dy;
            
            // ensure this move is within bounds
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                // bit of the destination square
                uint64_t to = 1ull << (yf * 8 + xf);

                // can't self-capture
                if (to & myPieces) {
                    continue;
                }

                bool capture = (to & oppPieces);
                
                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WN : BN), capture};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudolegal);
                
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
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
        for (auto &[dx, dy] : bishopMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // while move is in bounds, move in that direction
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);

                // prevent self captures
                if (to & myPieces) {
                    break;
                }

                bool capture = (to & oppPieces);

                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WB : BB), capture};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudolegal);
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
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
        for (auto &[dx, dy] : rookMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // while move is in bounds, move in that direction
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                
                // prevent self captures
                if (to & myPieces) {
                    break;
                }

                bool capture = (to & oppPieces);

                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WR : BR), capture};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudolegal);
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
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
        for (auto &[dx, dy] : queenMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // while move is in bounds, move in that direction
            while (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);

                // prevent self captures
                if (to & myPieces) {
                    break;
                }

                bool capture = (to & oppPieces);

                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WQ : BQ), capture};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudolegal);
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
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
        for (auto &[dx, dy] : kingMoves) {
            xf = xi + dx;
            yf = yi + dy;

            // ensure this move is within bounds
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                uint64_t to = 1ull << (yf * 8 + xf);
                
                // can't self-capture
                if (to & myPieces) {
                    continue;
                }

                bool capture = (to & oppPieces);

                Move pseudomove{{xi, yi}, {xf, yf}, (white ? WK : BK), capture};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudomove);
                if (!tempBoard.attacked({xf, yf}, !white)) {
                    moves.push_back(pseudomove);
                }
            }
        }

        // white kingside castle
        // TODO: test for check violations
        if (white && !whiteKingMoved && !whiteRookAMoved) {
            if ((emptySquares & (1ull << 61)) && (emptySquares & (1ull << 62))) {
                moves.push_back({{4, 7}, {6, 7}, WK, false});
            }
        }

        // white queenside castle
        if (white && !whiteKingMoved && !whiteRookHMoved) {
            if ((emptySquares & (1ull << 57)) && (emptySquares & (1ull << 58)) && (emptySquares & (1ull << 59))) {
                moves.push_back({{4, 7}, {2, 7}, WK, false});
            }
        }
        
        // black kingside castle
        if (!white && !blackKingMoved && !blackRookHMoved) {
            if ((emptySquares& (1ull << 5)) && (emptySquares& (1ull << 6))) {
                moves.push_back({{4, 0}, {6, 0}, BK, false});
            }
        }
        
        // black queenside castle
        if (!white && !blackKingMoved && !blackRookAMoved) {
            if ((emptySquares& (1ull << 1)) && (emptySquares& (1ull << 2)) && (emptySquares& (1ull << 3))) {
                moves.push_back({{4, 0}, {2, 0}, BK, false});
            }
        }

        // clear this king from the bitboard (unecessary)
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
