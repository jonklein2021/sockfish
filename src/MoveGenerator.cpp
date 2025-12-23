#include "MoveGenerator.h"

#include "Move.h"
#include "Position.h"

uint64_t MoveGenerator::computeAllSidesAttacks() const {
    return computeAllSidesAttacks(position.getWhoseTurn());
}

uint64_t MoveGenerator::computeAllSidesAttacks(Color color) const {
    uint64_t attacks = 0;
    const int startIndex = (color == WHITE) ? 0 : 6;
    const int endIndex = (color == WHITE) ? 6 : 12;
    for (int p = startIndex; p < endIndex; p++) {
        attacks |= computePieceAttacks(static_cast<Piece>(p));
    }
    return attacks;
}

uint64_t MoveGenerator::computePieceAttacks(Piece piece) const {
    uint64_t attacks = 0;
    uint64_t pieceBitboard = pieceBits[piece];
    while (pieceBitboard) {
        const uint64_t pieceBit = pieceBitboard & -pieceBitboard;  // isolate the LSB

        // compute attacks for this piece
        switch (piece) {
            case WP:
            case BP: attacks |= computePawnAttacks(pieceBit, piece == WP); break;
            case WN:
            case BN: attacks |= computeKnightAttacks(pieceBit); break;
            case WB:
            case BB: attacks |= computeBishopAttacks(pieceBit); break;
            case WR:
            case BR: attacks |= computeRookAttacks(pieceBit); break;
            case WQ:
            case BQ: attacks |= computeQueenAttacks(pieceBit); break;
            case WK:
            case BK: attacks |= computeKingAttacks(pieceBit); break;
            default: return 0;
        }

        pieceBitboard ^= pieceBit;  // remove the LSB from the bitboard
    }
    return attacks;
}

uint64_t MoveGenerator::computePawnAttacks(const uint64_t squareBit, const bool white) const {
    if (white) {
        return ((squareBit >> 7) & not_file_a) |
               ((squareBit >> 9) & not_file_h);  // up right, up left
    } else {
        return ((squareBit << 7) & not_file_h) |
               ((squareBit << 9) & not_file_a);  // down left, down right
    }
}

uint64_t MoveGenerator::computeKnightAttacks(const uint64_t squareBit) const {
    const uint64_t ddl = (squareBit & not_rank_12 & not_file_a) << 15;  // down 2, left 1
    const uint64_t ddr = (squareBit & not_rank_12 & not_file_h) << 17;  // down 2, right 1
    const uint64_t drr = (squareBit & not_rank_1 & not_file_gh) << 10;  // down 1, right 2
    const uint64_t dll = (squareBit & not_rank_1 & not_file_ab) << 6;   // down 1, left 2
    const uint64_t uur = (squareBit & not_rank_78 & not_file_h) >> 15;  // up 2, right 1
    const uint64_t uul = (squareBit & not_rank_78 & not_file_a) >> 17;  // up 2, left 1
    const uint64_t ull = (squareBit & not_rank_8 & not_file_ab) >> 10;  // up 1, left 2
    const uint64_t urr = (squareBit & not_rank_8 & not_file_gh) >> 6;   // up 1, right 2

    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

uint64_t MoveGenerator::computeBishopAttacks(const uint64_t squareBit) const {
    uint64_t attacks = 0;

    // initial row and column
    // N.B: (0, 0) is top left corner
    const auto [r0, c0] = bitToCoords(squareBit);

    // up right
    for (int r = r0 + 1, c = c0 + 1; r <= 7 && c <= 7; r++, c++) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, c = c0 - 1; r <= 7 && c >= 0; r++, c--) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    // down right
    for (int r = r0 - 1, c = c0 + 1; r >= 0 && c <= 7; r--, c++) {
        const uint64_t attackedSquare = 1ull << xyToSquare(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, c = c0 - 1; r >= 0 && c >= 0; r--, c--) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    return attacks;
}

uint64_t MoveGenerator::computeRookAttacks(const uint64_t squareBit) const {
    uint64_t attacks = 0;

    // initial row and column
    // N.B: (0, 0) is top left corner
    const auto [r0, c0] = bitToCoords(squareBit);

    // up
    for (int r = r0 + 1; r <= 7; r++) {
        const uint64_t attackedSquare = coordsToBit(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    // down
    for (int r = r0 - 1; r >= 0; r--) {
        const uint64_t attackedSquare = coordsToBit(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    // left
    for (int c = c0 - 1; c >= 0; c--) {
        const uint64_t attackedSquare = coordsToBit(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    // right
    for (int c = c0 + 1; c <= 7; c++) {
        const uint64_t attackedSquare = coordsToBit(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(ALL)) {
            break;
        }
    }

    return attacks;
}

uint64_t MoveGenerator::computeQueenAttacks(const uint64_t squareBit) const {
    return computeBishopAttacks(squareBit) | computeRookAttacks(squareBit);  // heheh
}

uint64_t MoveGenerator::computeKingAttacks(const uint64_t squareBit) const {
    const uint64_t d = (squareBit & not_rank_1) << 8;                // down
    const uint64_t u = (squareBit & not_rank_8) >> 8;                // up
    const uint64_t l = (squareBit & not_file_a) >> 1;                // left
    const uint64_t r = (squareBit & not_file_h) << 1;                // right
    const uint64_t dl = (squareBit & not_rank_1 & not_file_a) << 7;  // down left
    const uint64_t dr = (squareBit & not_rank_1 & not_file_h) << 9;  // down right
    const uint64_t ul = (squareBit & not_rank_8 & not_file_a) >> 9;  // up left
    const uint64_t ur = (squareBit & not_rank_8 & not_file_h) >> 7;  // up right

    return d | u | l | r | dl | dr | ul | ur;
}

/**
 * Returns return if and only if the given square is under attack by the given
 * side
 * @param
 * squareBit The square to check
 * @param white The attacking side. True for white, false for
 * black
 */
bool MoveGenerator::underAttack(const uint64_t squareBit, const bool white) const {
    return squareBit & (white ? (pieceAttacks[WP] | pieceAttacks[WN] | pieceAttacks[WB] |
                                 pieceAttacks[WR] | pieceAttacks[WQ] | pieceAttacks[WK])
                              : (pieceAttacks[BP] | pieceAttacks[BN] | pieceAttacks[BB] |
                                 pieceAttacks[BR] | pieceAttacks[BQ] | pieceAttacks[BK]));
}

/**
 * Returns return if and only if the given square is under attack by the
 * opponent
 * @param
 * squareBit The square to check
 */
bool MoveGenerator::underAttack(const uint64_t squareBit) const {
    return underAttack(squareBit, !whiteToMove);
}

bool MoveGenerator::insufficientMaterial() const {
    // count all pieces
    const int pieceCount[12] = {
        __builtin_popcountll(pieces[WP]),
        __builtin_popcountll(pieces[WN]),
        __builtin_popcountll(pieces[WB]),
        __builtin_popcountll(pieces[WR]),
        __builtin_popcountll(pieces[WQ]),
        1,  // always 1 white king
        __builtin_popcountll(pieces[BP]),
        __builtin_popcountll(pieces[BN]),
        __builtin_popcountll(pieces[BB]),
        __builtin_popcountll(pieces[BR]),
        __builtin_popcountll(pieces[BQ]),
        1  // always 1 black king
    };

    // note: consider testing for king + two knights vs king

    // insufficient material if at most one knight or bishop is left per side
    return !(pieceCount[WN] & pieceCount[WB]) & !(pieceCount[BN] & pieceCount[BB]) &
           !(pieceCount[WP] | pieceCount[WR] | pieceCount[WQ] | pieceCount[BP] | pieceCount[BR] |
             pieceCount[BQ]);
}

bool MoveGenerator::isTerminal() const {
    const int n = md.history.size();
    return md.movesSinceCapture >= 100 ||
           (n >= 9 && md.history[n - 1] == md.history[n - 5] &&
            md.history[n - 5] == md.history[n - 9]) ||
           insufficientMaterial() || generateMoves().empty();
}

bool MoveGenerator::isCheck() const {
    // get position of king
    const uint64_t king = pieces[whiteToMove ? WK : BK];

    // check if king is under attack
    return underAttack(king);
}

bool MoveGenerator::isMoveLegal(Position &copy,
                                const uint64_t kingBit,
                                const Move &move,
                                bool white) const {
    // simulate move on copied state
    Metadata md = copy.makeMove(move);

    // test if king is in check after move
    bool isOurKingInCheck = underAttack(kingBit, !white);

    // unmake move to preserve original state
    copy.unmakeMove(move, md);

    // return true iff the move is not illegal
    return !isOurKingInCheck;
}

Move *generateLegal(Position &pos) {
    std::vector<Move> moves;

    // stores a copy of the bitboard of a selected piece
    Bitboard pieceBitboard;

    // bit that signifies a piece's initial location
    Bitboard fromBit;

    // bit that signifies a piece's final location
    Bitboard toBit;

    Piece capturedPiece;

    std::vector<PieceType> promotionPieces;
    std::vector<PieceType> oppPieces;
    Piece PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, OPP_PAWN;
    int SIDE;
    int OPP;
    if (whiteToMove) {
        promotionPieces = {WQ, WR, WB, WN};
        oppPieces = {BP, BN, BB, BR, BQ, BK};
        PAWN = WP;
        KNIGHT = WN;
        BISHOP = WB;
        ROOK = WR;
        QUEEN = WQ;
        KING = WK;
        OPP_PAWN = BP;
        SIDE = WHITE;
        OPP = BLACK;
    } else {
        promotionPieces = {BQ, BR, BB, BN};
        oppPieces = {WP, WN, WB, WR, WQ, WK};
        PAWN = BP;
        KNIGHT = BN;
        BISHOP = BB;
        ROOK = BR;
        QUEEN = BQ;
        KING = BK;
        OPP_PAWN = WP;
        SIDE = BLACK;
        OPP = WHITE;
    }

    // used to check if a move is legal
    Position copy(*this);
    Move candidate;
    const uint64_t kingBit = pieces[KING];

    /* PAWN MOVES */
    pieceBitboard = pieces[PAWN];
    if (whiteToMove) {
        while (pieceBitboard) {
            fromBit = 1ull << indexOfLs1b(pieceBitboard);
            const sf::Vector2<int> from = bitToCoords(fromBit);

            // single pawn moves
            toBit = fromBit >> 8;
            sf::Vector2<int> to = bitToCoords(toBit);
            candidate = {from, to, PAWN};
            if ((board.getOccupancy(NEITHER) & toBit) &&
                isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                // promotion check
                if (toBit & rank8) {
                    for (PieceType pt : promotionPieces) {
                        moves.push_back({from, to, PAWN, None, pt});
                    }
                } else {
                    moves.push_back(candidate);
                }
            }

            // double pawn moves
            uint64_t overBit = fromBit >> 8;
            toBit = fromBit >> 16;
            to = bitToCoords(toBit);
            candidate = {from, to, PAWN};
            if ((rank2 & fromBit) && (board.getOccupancy(NEITHER) & overBit) &&
                (board.getOccupancy(NEITHER) & toBit) &&
                isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }

            // pawn captures up and right
            toBit = (fromBit & not_file_h) >> 7;
            to = bitToCoords(toBit);
            if (toBit & (board.getOccupancy(OPP) | (1ull << md.enPassantBitOffset))) {

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank8) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            // pawn captures up and left
            toBit = (fromBit & not_file_a) >> 9;
            if (toBit & (board.getOccupancy(OPP) | (1 << md.enPassantBitOffset))) {
                to = bitToCoords(toBit);

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank8) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            pieceBitboard &= pieceBitboard - 1;
        }
    } else {
        while (pieceBitboard) {
            fromBit = 1ull << indexOfLs1b(pieceBitboard);
            const sf::Vector2<int> from = bitToCoords(fromBit);

            // single pawn moves
            toBit = fromBit << 8;
            if (board.getOccupancy(NEITHER) & toBit) {
                sf::Vector2<int> to = bitToCoords(toBit);

                // promotion check
                if (toBit & rank1) {
                    for (PieceType pt : promotionPieces) {
                        candidate = {from, to, PAWN, None, pt};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    candidate = {from, to, PAWN};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            // double pawn moves
            uint64_t overBit = fromBit << 8;
            toBit = fromBit << 16;
            if ((rank7 & fromBit) && (board.getOccupancy(NEITHER) & overBit) &&
                (board.getOccupancy(NEITHER) & toBit)) {
                sf::Vector2<int> to = bitToCoords(toBit);
                candidate = {from, to, PAWN};
                if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                    moves.push_back(candidate);
                }
            }

            // pawn captures down and right
            toBit = (fromBit & not_file_h) << 9;
            if (toBit & (board.getOccupancy(OPP) | (1 << md.enPassantBitOffset))) {
                sf::Vector2<int> to = bitToCoords(toBit);

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank1) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            // pawn captures down and left
            toBit = (fromBit & not_file_a) << 7;
            if (toBit & (board.getOccupancy(OPP) | (1 << md.enPassantBitOffset))) {
                sf::Vector2<int> to = bitToCoords(toBit);

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank1) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            pieceBitboard &= pieceBitboard - 1;
        }
    }

    /* KNIGHT MOVES */
    pieceBitboard = pieces[KNIGHT];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        // down 2, left 1
        if ((toBit = ((fromBit & not_rank_12 & not_file_a) << 15)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // down 2, right 1
        if ((toBit = ((fromBit & not_rank_12 & not_file_h) << 17)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // down 1, right 2
        if ((toBit = ((fromBit & not_rank_1 & not_file_gh) << 10)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // down 1, left 2
        if ((toBit = ((fromBit & not_rank_1 & not_file_ab) << 6)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 2, right 1
        if ((toBit = ((fromBit & not_rank_78 & not_file_h) >> 15)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 2, left 1
        if ((toBit = ((fromBit & not_rank_78 & not_file_a) >> 17)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 1, left 2
        if ((toBit = ((fromBit & not_rank_8 & not_file_ab) >> 10)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 1, right 2
        if ((toBit = ((fromBit & not_rank_8 & not_file_gh) >> 6)) &&
            (toBit & ~board.getOccupancy(SIDE))) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* BISHOP MOVES */
    pieceBitboard = pieces[BISHOP];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;
        const uint64_t otherPieces = board.getOccupancy(ALL) & ~fromBit;

        // up right
        while (filteredMask = (toBit & not_rank_8 & not_file_h & ~otherPieces)) {
            toBit = filteredMask >> 7;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // up left
        while (filteredMask = (toBit & not_rank_8 & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 9;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down right
        while (filteredMask = (toBit & not_rank_1 & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 9;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down left
        while (filteredMask = (toBit & not_rank_1 & not_file_a & ~otherPieces)) {
            toBit = filteredMask << 7;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* ROOK MOVES */
    pieceBitboard = pieces[ROOK];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;
        const uint64_t otherPieces = board.getOccupancy(ALL) & ~fromBit;

        // up
        while (filteredMask = (toBit & not_rank_8 & ~otherPieces)) {
            toBit = filteredMask >> 8;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down
        while (filteredMask = (toBit & not_rank_1 & ~otherPieces)) {
            toBit = filteredMask << 8;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // left
        while (filteredMask = (toBit & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 1;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // right
        while (filteredMask = (toBit & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 1;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* QUEEN MOVES */
    pieceBitboard = pieces[QUEEN];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;
        const uint64_t otherPieces = board.getOccupancy(ALL) & ~fromBit;

        // up
        while (filteredMask = (toBit & not_rank_8 & ~otherPieces)) {
            toBit = filteredMask >> 8;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down
        while (filteredMask = (toBit & not_rank_1 & ~otherPieces)) {
            toBit = filteredMask << 8;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // left
        while (filteredMask = (toBit & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 1;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // right
        while (filteredMask = (toBit & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 1;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // up right
        while (filteredMask = (toBit & not_rank_8 & not_file_h & ~otherPieces)) {
            toBit = filteredMask >> 7;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // up left
        while (filteredMask = (toBit & not_rank_8 & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 9;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down right
        while (filteredMask = (toBit & not_rank_1 & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 9;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down left
        while (filteredMask = (toBit & not_rank_1 & not_file_a & ~otherPieces)) {
            toBit = filteredMask << 7;
            if (toBit & board.getOccupancy(SIDE)) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* KING MOVES */
    fromBit = pieces[KING];
    const sf::Vector2<int> from = bitToCoords(fromBit);

    // standard moves
    if ((toBit = ((fromBit & not_rank_1) << 8)) && (toBit & ~board.getOccupancy(SIDE))) {
        // down
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_8) >> 8)) && (toBit & ~board.getOccupancy(SIDE))) {
        // up
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_file_a) >> 1)) && (toBit & ~board.getOccupancy(SIDE))) {
        // left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_file_h) << 1)) && (toBit & ~board.getOccupancy(SIDE))) {
        // right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_1 & not_file_a) << 7)) &&
        (toBit & ~board.getOccupancy(SIDE))) {
        // down left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_1 & not_file_h) << 9)) &&
        (toBit & ~board.getOccupancy(SIDE))) {
        // down right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_8 & not_file_a) >> 9)) &&
        (toBit & ~board.getOccupancy(SIDE))) {
        // up left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_8 & not_file_h) >> 7)) &&
        (toBit & ~board.getOccupancy(SIDE))) {
        // up right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    // castling

    // white kingside castle
    if (whiteToMove && (md.castleRights & 0b0001) && (board.getOccupancy(NEITHER) & (1ull << f1)) &&
        (board.getOccupancy(NEITHER) & (1ull << g1)) && !underAttack(fromBit) &&  // e1
        !underAttack(1ull << f1) && !underAttack(1ull << g1)) {
        moves.push_back({from, {6, 7}, KING, NO_PIECE, NO_PIECE, true, false});
    }

    // white queenside castle
    if (whiteToMove && (md.castleRights & 0b0010) && (board.getOccupancy(NEITHER) & (1ull << d1)) &&
        (board.getOccupancy(NEITHER] & (1ull << c1)) && (occupancies[NEITHER) & (1ull << b1)) &&
        !underAttack(fromBit) &&  // e1
        !underAttack(1ull << d1) && !underAttack(1ull << c1)) {
        moves.push_back({from, {2, 7}, KING, NO_PIECE, NO_PIECE, false, true});
    }

    // black kingside castle
    if (!whiteToMove && (md.castleRights & 0b0100) &&
        (board.getOccupancy(NEITHER) & (1ull << f8)) &&
        (board.getOccupancy(NEITHER) & (1ull << g8)) && !underAttack(fromBit) &&  // e8
        !underAttack(1ull << f8) && !underAttack(1ull << g8)) {
        moves.push_back({from, {6, 0}, KING, NO_PIECE, NO_PIECE, true, false});
    }

    // black queenside castle
    if (!whiteToMove && (md.castleRights & 0b1000) && (board.getOccupancy(NEITHER) & (1ull << d8)) &&
        (board.getOccupancy(NEITHER] & (1ull << c8)) && (occupancies[NEITHER) & (1ull << b8)) &&
        !underAttack(fromBit) &&  // e8
        !underAttack(1ull << d8) && !underAttack(1ull << c8)) {
        moves.push_back({from, {2, 0}, KING, NO_PIECE, NO_PIECE, false, true});
    }

    return moves;
}

Move *generatePseudolegal(Position &pos) {
    return nullptr;
}
