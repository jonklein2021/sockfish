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

void BitBoard::print() const {
    for (int i = 0; i < 12; i++) {
        print(static_cast<PieceType>(i));
    }
}

void BitBoard::print(PieceType p) const {
    printU64(pieceBits[p]);
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
    // basic moves
    uint64_t from = 1ull << (move.from.y * 8 + move.from.x);
    uint64_t to = 1ull << (move.to.y * 8 + move.to.x);
    uint64_t fromTo = from | to;
    
}

std::vector<Move> generateMoves(const GameState& state) {
    std::vector<Move> moves;
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

    // initial and final positions of each piece
    int xi, yi, xf, yf;

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
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WN : BN), true, false, false, ' '});
                } else if (to & emptySquares) { // necessary so we don't move to our own piece
                    // move is not a capture
                    moves.push_back({{xi, yi}, {xf, yf}, (white ? WN : BN), false, false, false, ' '});
                }
            }
        }

        // clear this knight from the bitboard
        knights &= knights - 1;
    }   
    
    return moves;
}
