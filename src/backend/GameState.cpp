#include "GameState.h"
#include "constants.h"

GameState::GameState() : board(), whiteToMove(true), whiteKingMoved(false), blackKingMoved(false),
                  whiteRookAMoved(false), whiteRookHMoved(false), blackRookAMoved(false), blackRookHMoved(false) {}
                
GameState::GameState(const std::string &fen) : board(fenToBitBoard(fen)), whiteToMove(true), whiteKingMoved(false), blackKingMoved(false),
    whiteRookAMoved(false), whiteRookHMoved(false), blackRookAMoved(false), blackRookHMoved(false) {}

GameState::GameState(BitBoard board, bool whiteToMove, bool whiteKingMoved, bool blackKingMoved,
            bool whiteRookAMoved, bool whiteRookHMoved, bool blackRookAMoved, bool blackRookHMoved)
    : board(board), whiteToMove(whiteToMove), whiteKingMoved(whiteKingMoved), blackKingMoved(blackKingMoved),
        whiteRookAMoved(whiteRookAMoved), whiteRookHMoved(whiteRookHMoved), blackRookAMoved(blackRookAMoved), blackRookHMoved(blackRookHMoved) {}

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
        if (yf >= 1 && yf < 7) { // handle pawn promotion separately
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

        // pawn promotion
        if ((white && yf == 0) || (!white && yf == 7)) {
            for (PieceType p : {WQ, WR, WB, WN}) {
                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP), p, false};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudolegal);
                
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

        // TODO: en passant

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

        // generate all normal king moves
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
        if (white && !whiteKingMoved && !whiteRookAMoved) {
            if ((emptySquares & (1ull << 61)) && (emptySquares & (1ull << 62))) {
                Move pseudomove{{4, 7}, {6, 7}, WK, false};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudomove);
                if (!tempBoard.attacked(kingPos, !white) && !tempBoard.attacked({5, 7}, !white) && !tempBoard.attacked({6, 7}, !white)) {
                    moves.push_back(pseudomove);
                }
            }
        }

        // white queenside castle
        if (white && !whiteKingMoved && !whiteRookHMoved) {
            if ((emptySquares & (1ull << 57)) && (emptySquares & (1ull << 58)) && (emptySquares & (1ull << 59))) {
                Move pseudomove{{4, 7}, {2, 7}, WK, false};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudomove);
                if (!tempBoard.attacked(kingPos, !white) && !tempBoard.attacked({3, 7}, !white) && !tempBoard.attacked({2, 7}, !white)) {
                    moves.push_back(pseudomove);
                }
            }

            
        }
        
        // black kingside castle
        if (!white && !blackKingMoved && !blackRookHMoved) {
            if ((emptySquares& (1ull << 5)) && (emptySquares& (1ull << 6))) {
                Move pseudomove{{4, 0}, {6, 0}, BK, false};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudomove);
                if (!tempBoard.attacked(kingPos, !white) && !tempBoard.attacked({5, 0}, !white) && !tempBoard.attacked({6, 0}, !white)) {
                    moves.push_back(pseudomove);
                }
            }
        }
        
        // black queenside castle
        if (!white && !blackKingMoved && !blackRookAMoved) {
            if ((emptySquares& (1ull << 1)) && (emptySquares& (1ull << 2)) && (emptySquares& (1ull << 3))) {
                Move pseudomove{{4, 0}, {2, 0}, BK, false};
                BitBoard tempBoard(board);
                tempBoard.applyMove(pseudomove);
                if (!tempBoard.attacked(kingPos, !white) && !tempBoard.attacked({5, 0}, !white) && !tempBoard.attacked({6, 0}, !white)) {
                    moves.push_back(pseudomove);
                }
            }
        }

        // clear this king from the bitboard (unecessary)
        king &= king - 1;
    }

    
    return moves;
}
