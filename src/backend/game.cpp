#include "game.h"

BitBoard fenToBitBoard(const std::string& fen) {
    BitBoard board;
    int x = 0, y = 0;
    for (const char& c : fen) {
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square
            x += c - '0';
        } else { // piece
            uint64_t* piece = nullptr;
            switch (c) {
                case 'P': piece = &board.wp; break;
                case 'N': piece = &board.wn; break;
                case 'B': piece = &board.wb; break;
                case 'R': piece = &board.wr; break;
                case 'Q': piece = &board.wq; break;
                case 'K': piece = &board.wk; break;
                case 'p': piece = &board.bp; break;
                case 'n': piece = &board.bn; break;
                case 'b': piece = &board.bb; break;
                case 'r': piece = &board.br; break;
                case 'q': piece = &board.bq; break;
                case 'k': piece = &board.bk; break;
            }

            // set this piece's bit at the correct position
            *piece |= 1ULL << (y * 8 + x);
            x++;
        }
    }
    return board;
}

std::string bitBoardToFen(const BitBoard& board) {

}