#include "GameState.h"
#include "constants.h"

#include <iostream>

GameState::GameState() : GameState(defaultFEN) {}

GameState::GameState(const std::string &fen) {
    // init metadata
    md = {
        false, // whiteKCastle
        false, // whiteQCastle
        false, // blackKCastle
        false, // blackQCastle
        {-1, -1}, // enPassantSquare
        0, // movesSinceCapture
        {} // history (not used yet)
    };
    
    // 1: position data
    size_t i = 0;
    int x = 0, y = 0;
    for (; i < fen.size() && fen[i] != ' '; i++) {
        const char c = fen[i];
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square; skip x squares
            x += c - '0';
        } else { // piece
            // set this piece's bit at the correct position
            PieceType temp = fenPieceMap.at(c);
            board.pieceBits[temp] |= 1ull << (y * 8 + x);
            x++;
        }
    }

    if (i >= fen.size()) return;

    // 2: whose turn it is
    whiteToMove = fen[++i] == 'w';

    // 3: castling rights
    i += 2;
    for (; fen[i] != ' ' && i < fen.size(); i++) {
        if (fen[i] == 'K') md.whiteKCastleRights = true;
        if (fen[i] == 'Q') md.whiteQCastleRights = true;
        if (fen[i] == 'k') md.blackKCastleRights = true;
        if (fen[i] == 'q') md.blackQCastleRights = true;
    }

    if (i >= fen.size()) return;

    // 4: en passant square
    if (fen[++i] != '-') {
        int file = fen[i] - 'a';
        int rank = '8' - fen[i + 1];
        md.enPassantSquare = {file, rank};
    }

    if (i >= fen.size()) return;

    // 5: halfmove clock
    i += 2;
    std::string halfmoveClock = "";
    for (; i < fen.size() && fen[i] != ' '; i++) {
        halfmoveClock += fen[i];
    }
    md.movesSinceCapture = halfmoveClock.empty() ? 0 : std::stoi(halfmoveClock);

    // 6: fullmove number (not used)

}

Metadata GameState::makeMove(const Move &move) {
    // save metadata about this state before making move
    Metadata oldMD = md;

    // make changes to board representation
    board.makeMove(move);

    // covers standard king moves and castling
    if (whiteToMove) {
        if (move.piece == WK) {
            md.whiteKCastleRights = false;
            md.whiteQCastleRights = false;
        }
    } else {
        if (move.piece == BK) {
            md.blackKCastleRights = false;
            md.blackQCastleRights = false;
        }
    }

    // rook captures
    if (move.to.x == 0 && move.to.y == 0) md.blackQCastleRights = false;
    if (move.to.x == 7 && move.to.y == 0) md.blackKCastleRights = false;
    if (move.to.x == 0 && move.to.y == 7) md.whiteQCastleRights = false;
    if (move.to.x == 7 && move.to.y == 7) md.whiteKCastleRights = false;
    
    // rook moves
    if (move.piece == BR && move.from.x == 0) md.blackQCastleRights = false;
    if (move.piece == BR && move.from.x == 7) md.blackKCastleRights = false;
    if (move.piece == WR && move.from.x == 0) md.whiteQCastleRights = false;
    if (move.piece == WR && move.from.x == 7) md.whiteKCastleRights = false;

    // update en passant square if pawn has moved two squares
    if (move.piece == WP && move.from.y == 6 && move.to.y == 4)
        md.enPassantSquare = {move.from.x, 5};
    else if (move.piece == BP && move.from.y == 1 && move.to.y == 3)
        md.enPassantSquare = {move.from.x, 2};
    else
        md.enPassantSquare = {-1, -1};

    // update 50 move rule
    if (move.capturedPiece != None)
        md.movesSinceCapture = 0;
    else
        md.movesSinceCapture++;

    // change turns
    whiteToMove = !whiteToMove;
    
    return oldMD;
}

void GameState::unmakeMove(const Move &move, const Metadata &prevMD) {
    // revert board state
    board.unmakeMove(move);

    // restore turn order
    whiteToMove = !whiteToMove;

    // restore metadata
    md = prevMD;
}

bool GameState::underAttack(sf::Vector2<int> square) const {
    return board.attacked(square, !whiteToMove);
}

bool GameState::isTerminal() const {
    // TODO: add threefold repetition check and make this more efficient
    return generateMoves().empty() || md.movesSinceCapture >= 100 || board.isDraw();
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

    // pieces that we can capture
    uint64_t oppArray[5] = {oppPawns, oppKnights, oppBishops, oppRooks, oppQueens};
    
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
                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP)};
                BitBoard tempBoard(board);
                tempBoard.makeMove(pseudolegal);

                // only add if king is not under attack by opponent in resulting position
                if (!tempBoard.attacked(kingPos, !white)) {    
                    // pawn promotion
                    if ((white && yf == 0) || (!white && yf == 7)) {
                        for (PieceType p : (white ? promotionPiecesWhite : promotionPiecesBlack)) {
                            moves.push_back(Move{{xi, yi}, {xf, yf}, (white ? WP : BP), None, p});
                        }
                    } else {
                        moves.push_back(pseudolegal);
                    }
                }
            }
        }

        // move two squares forward only if pawn is in its starting position
        if ((white && yi == 6) || (!white && yi == 1)) {
            uint64_t over = 1ull << ((yi + direction) * 8 + xf); // square pawn jumps over
            uint64_t to = 1ull << ((yf + direction) * 8 + xf); // destination square
            yf += direction;
            
            // both `over` and `to` must be empty for a pawn to move 2 squares
            if ((emptySquares & over) && (emptySquares & to)) {
                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP)};
                BitBoard tempBoard(board);
                tempBoard.makeMove(pseudolegal);

                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
                }
            }
        }

        // pawn standard capture moves
        for (auto &[dx, dy] : pawnCaptures) {
            xf = xi + dx;
            yf = yi + dy * direction;
            if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                to = 1ull << (yf * 8 + xf);

                for (int i = 0; i < 5; i++) {
                    if (to & oppArray[i]) {
                        Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP), static_cast<PieceType>(white ? i+6 : i)};
                        BitBoard tempBoard(board);
                        tempBoard.makeMove(pseudolegal);
                        
                        // skip if king is under attack in resulting position
                        if (tempBoard.attacked(kingPos, !white)) {
                            continue;
                        }
                        
                        // check for promotion
                        if ((white && yf == 0) || (!white && yf == 7)) {
                            for (PieceType p : (white ? promotionPiecesWhite : promotionPiecesBlack)) {
                                pseudolegal.promotionPiece = p;
                                moves.push_back(pseudolegal);
                            }
                        } else {
                            moves.push_back(pseudolegal);
                        }
                    }
                }
            }
        }

        // en passant
        if (md.enPassantSquare.x != -1) {
            for (int dx : {-1, 1}) {
                xf = xi + dx;
                yf = yi + direction;
                if (xf >= 0 && xf < 8 && yf >= 0 && yf < 8) {
                    to = 1ull << (yf * 8 + xf);
                    if (to & (1ull << (md.enPassantSquare.y * 8 + md.enPassantSquare.x))) {
                        Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WP : BP), (white ? BP : WP), None, false, false, true};
                        BitBoard tempBoard(board);
                        tempBoard.makeMove(pseudolegal);
                        
                        if (!tempBoard.attacked(kingPos, !white)) {
                            moves.push_back(pseudolegal);
                        }
                    }
                }
            }
        }

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

                // check if this move is a capture
                PieceType captured = None;
                for (int i = 0; i < 5; i++) {
                    if (to & oppArray[i]) {
                        captured = static_cast<PieceType>(white ? i+6 : i);
                        break;
                    }
                }
                
                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WN : BN), captured};
                BitBoard tempBoard(board);
                tempBoard.makeMove(pseudolegal);
                
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

                // check if this move is a capture
                PieceType captured = None;
                for (int i = 0; i < 5; i++) {
                    if (to & oppArray[i]) {
                        captured = static_cast<PieceType>(white ? i+6 : i);
                        break;
                    }
                }

                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WB : BB), captured};
                BitBoard tempBoard(board);
                tempBoard.makeMove(pseudolegal);
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
                }

                // stop after a capture
                if (captured != None) {
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

                // check if this move is a capture
                PieceType captured = None;
                for (int i = 0; i < 5; i++) {
                    if (to & oppArray[i]) {
                        captured = static_cast<PieceType>(white ? i+6 : i);
                        break;
                    }
                }

                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WR : BR), captured};
                BitBoard tempBoard(board);
                tempBoard.makeMove(pseudolegal);
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
                }
                
                // stop after a capture
                if (captured != None) {
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

                // check if this move is a capture
                PieceType captured = None;
                for (int i = 0; i < 5; i++) {
                    if (to & oppArray[i]) {
                        captured = static_cast<PieceType>(white ? i+6 : i);
                        break;
                    }
                }

                Move pseudolegal{{xi, yi}, {xf, yf}, (white ? WQ : BQ), captured};
                BitBoard tempBoard(board);
                tempBoard.makeMove(pseudolegal);
                if (!tempBoard.attacked(kingPos, !white)) {
                    moves.push_back(pseudolegal);
                }

                // stop after a capture
                if (captured != None) {
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

                // check if this move is a capture
                PieceType captured = None;
                for (int i = 0; i < 5; i++) {
                    if (to & oppArray[i]) {
                        captured = static_cast<PieceType>(white ? i+6 : i);
                        break;
                    }
                }

                Move pseudomove{{xi, yi}, {xf, yf}, (white ? WK : BK), captured};
                BitBoard tempBoard(board);
                tempBoard.makeMove(pseudomove);
                if (!tempBoard.attacked({xf, yf}, !white)) {
                    moves.push_back(pseudomove);
                }
            }
        }

        // {king square, rook square, squares in between}
        const uint64_t whiteKingsideCastle[4] = {1ull << 60, 1ull << 63, 1ull << 61, 1ull << 62};
        const uint64_t whiteQueensideCastle[5] = {1ull << 60, 1ull << 56, 1ull << 57, 1ull << 58, 1ull << 59};
        const uint64_t blackKingsideCastle[4] = {1ull << 4, 1ull << 7, 1ull << 5, 1ull << 6};
        const uint64_t blackQueensideCastle[5] = {1ull << 4, 1ull << 0, 1ull << 1, 1ull << 2, 1ull << 3};

        // white kingside castle
        if (white && md.whiteKCastleRights &&
            (emptySquares & whiteKingsideCastle[2]) &&
            (emptySquares & whiteKingsideCastle[3]) &&
            !board.attacked(kingPos, !white) && // {4, 7}
            !board.attacked({5, 7}, !white) &&
            !board.attacked({6, 7}, !white)
        ) {
          // no need to check for castling into check because we already
          // tested this square in the line above
          Move pseudomove{{4, 7}, {6, 7}, WK, None, None, true, false};
          moves.push_back(pseudomove);
        }

        // white queenside castle
        if (white && md.whiteQCastleRights &&
            (emptySquares & whiteQueensideCastle[2]) &&
            (emptySquares & whiteQueensideCastle[3]) &&
            (emptySquares & whiteQueensideCastle[4]) && 
            !board.attacked(kingPos, !white) && // {4, 7}
            !board.attacked({2, 7}, !white) &&
            !board.attacked({3, 7}, !white)
        ) {
          Move pseudomove{{4, 7}, {2, 7}, WK, None, None, false, true};
          moves.push_back(pseudomove);
        }

        // black kingside castle
        if (!white && md.blackKCastleRights &&
            (emptySquares & blackKingsideCastle[2]) &&
            (emptySquares & blackKingsideCastle[3]) &&
            !board.attacked(kingPos, !white) && // {4, 0}
            !board.attacked({5, 0}, !white) &&
            !board.attacked({6, 0}, !white)
        ) {
          Move pseudomove{{4, 0}, {6, 0}, BK, None, None, true, false};
          moves.push_back(pseudomove);
        }

        // black queenside castle
        if (!white && md.blackQCastleRights &&
            (emptySquares & blackQueensideCastle[2]) &&
            (emptySquares & blackQueensideCastle[3]) &&
            (emptySquares & blackQueensideCastle[4]) &&
            !board.attacked(kingPos, !white) && // {4, 0}
            !board.attacked({2, 0}, !white) &&
            !board.attacked({3, 0}, !white)
        ) {
          Move pseudomove{{4, 0}, {2, 0}, BK, None, None, false, true};
          moves.push_back(pseudomove);
        }

        // clear this king from the bitboard (unecessary)
        king &= king - 1;
    }

    
    return moves;
}


void GameState::print() const {
    board.prettyPrint();
    std::cout << "whiteToMove = " << whiteToMove << std::endl;
    std::cout << "whiteKCastleRights = " << md.whiteKCastleRights << std::endl;
    std::cout << "whiteQCastleRights = " << md.whiteQCastleRights << std::endl;
    std::cout << "blackKCastleRights = " << md.blackKCastleRights << std::endl;
    std::cout << "blackQCastleRights = " << md.blackQCastleRights << std::endl;
    std::cout << "enPassantSquare = " << md.enPassantSquare.x << ", " << md.enPassantSquare.y << std::endl;
    std::cout << "movesSinceCapture = " << md.movesSinceCapture << std::endl;
}