#include "MoveGenerator.h"

#include "Move.h"
#include "Position.h"

MoveGenerator::MoveGenerator(Position &pos)
    : position(pos) {}

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
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, c = c0 - 1; r <= 7 && c >= 0; r++, c--) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // down right
    for (int r = r0 - 1, c = c0 + 1; r >= 0 && c <= 7; r--, c++) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, c = c0 - 1; r >= 0 && c >= 0; r--, c--) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
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
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // down
    for (int r = r0 - 1; r >= 0; r--) {
        const uint64_t attackedSquare = coordsToBit(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // left
    for (int c = c0 - 1; c >= 0; c--) {
        const uint64_t attackedSquare = coordsToBit(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // right
    for (int c = c0 + 1; c <= 7; c++) {
        const uint64_t attackedSquare = coordsToBit(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
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

Piece MoveGenerator::getCapturedPiece(const uint64_t toBit,
                                      const std::vector<Piece> &oppPieces) const {
    for (Piece p : oppPieces) {
        if (pieceBits[p] & toBit) {
            return p;
        }
    }
    return NO_PIECE;
}

/**
 * Returns return if and only if the given square is under attack by the given
 * side
 * @param squareBit The square to check
 * @param white The attacking side. True for white, false for black
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
 * @param squareBit The square to check
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
    return nullptr;
}

Move *generatePseudolegal(Position &pos) {
    return nullptr;
}
