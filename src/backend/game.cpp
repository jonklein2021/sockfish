#include "game.h"

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

std::vector<Move> getLegalMoves(const GameState& state) {
    std::vector<Move> moves;
    bool white = state.whiteToMove;

    // get all pieces
    uint64_t pawns = white ? state.board.pieceBits[WP] : state.board.pieceBits[BP];
    uint64_t knights = white ? state.board.pieceBits[WN] : state.board.pieceBits[BN];
    uint64_t bishops = white ? state.board.pieceBits[WB] : state.board.pieceBits[BB];
    uint64_t rooks = white ? state.board.pieceBits[WR] : state.board.pieceBits[BR];
    uint64_t queens = white ? state.board.pieceBits[WQ] : state.board.pieceBits[BQ];
    uint64_t king = white ? state.board.pieceBits[WK] : state.board.pieceBits[BK];
    
    // may be useful for later
    uint64_t myPieces = pawns | knights | bishops | rooks | queens | king;
    uint64_t oppPieces = ~myPieces;
    uint64_t allPieces = myPieces | oppPieces;
    uint64_t emptySquares = ~allPieces;
    
    return moves;
}
