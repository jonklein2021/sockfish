#include "MoveGenerator.h"

#include "Move.h"
#include "Position.h"
#include "bit_tools.h"
#include "src/types.h"

MoveGenerator::MoveGenerator(std::shared_ptr<Position> _pos)
    : pos(_pos) {}

Bitboard MoveGenerator::computeAllSidesAttacks() {
    return computeAllSidesAttacks(pos->getSideToMove());
}

Bitboard MoveGenerator::computeAllSidesAttacks(Color color) {
    Bitboard attacks = 0;
    const int startIndex = (color == WHITE) ? 0 : 6;
    const int endIndex = (color == WHITE) ? 6 : 12;
    for (int p = startIndex; p < endIndex; p++) {
        attacks |= computePieceAttacks(static_cast<Piece>(p));
    }
    return attacks;
}

Bitboard MoveGenerator::computePieceAttacks(Piece piece) {
    Bitboard attacks = 0;
    Bitboard pieceBitboard = pos->board.getPieces(piece);
    while (pieceBitboard) {
        const Bitboard sqBB = pieceBitboard & -pieceBitboard;  // isolate the LSB
        const Square sq = Square(indexOfLs1b(sqBB));

        // compute attacks for this piece
        switch (piece) {
            case WP:
            case BP: attacks |= computePawnAttacks(sq, piece == WP ? WHITE : BLACK); break;
            case WN:
            case BN: attacks |= computeKnightAttacks(sq); break;
            case WB:
            case BB: attacks |= computeBishopAttacks(sq); break;
            case WR:
            case BR: attacks |= computeRookAttacks(sq); break;
            case WQ:
            case BQ: attacks |= computeQueenAttacks(sq); break;
            case WK:
            case BK: attacks |= computeKingAttacks(sq); break;
            default: return 0;
        }

        pieceBitboard ^= sqBB;  // remove the LSB from the bitboard
    }
    return attacks;
}

constexpr Bitboard MoveGenerator::computePawnAttacks(Square sq, Color side) {
    const Bitboard sqBB = 1 << sq;
    if (side == WHITE) {
        return ((sqBB >> 7) & not_file_a) | ((sqBB >> 9) & not_file_h);  // up right, up left
    } else {
        return ((sqBB << 7) & not_file_h) | ((sqBB << 9) & not_file_a);  // down left, down right
    }
}

constexpr Bitboard MoveGenerator::computeKnightAttacks(Square sq) {
    const Bitboard sqBB = 1 << sq;
    const Bitboard ddl = (sqBB & not_rank_12 & not_file_a) << 15;  // down 2, left 1
    const Bitboard ddr = (sqBB & not_rank_12 & not_file_h) << 17;  // down 2, right 1
    const Bitboard drr = (sqBB & not_rank_1 & not_file_gh) << 10;  // down 1, right 2
    const Bitboard dll = (sqBB & not_rank_1 & not_file_ab) << 6;   // down 1, left 2
    const Bitboard uur = (sqBB & not_rank_78 & not_file_h) >> 15;  // up 2, right 1
    const Bitboard uul = (sqBB & not_rank_78 & not_file_a) >> 17;  // up 2, left 1
    const Bitboard ull = (sqBB & not_rank_8 & not_file_ab) >> 10;  // up 1, left 2
    const Bitboard urr = (sqBB & not_rank_8 & not_file_gh) >> 6;   // up 1, right 2

    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

constexpr Bitboard MoveGenerator::computeBishopAttacks(Square sq) {
    Bitboard attacks = 0;
    const Board board = pos->board;

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up right
    for (int r = r0 + 1, c = c0 + 1; r <= 7 && c <= 7; r++, c++) {
        const Bitboard attackedSquare = (1 << xyToSquare(c, r));
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, c = c0 - 1; r <= 7 && c >= 0; r++, c--) {
        const Bitboard attackedSquare = (1 << xyToSquare(c, r));
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // down right
    for (int r = r0 - 1, c = c0 + 1; r >= 0 && c <= 7; r--, c++) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, c = c0 - 1; r >= 0 && c >= 0; r--, c--) {
        const Bitboard attackedSquare = 1ull << xyToSquare(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    return attacks;
}

constexpr Bitboard MoveGenerator::computeRookAttacks(Square sq) {
    Bitboard attacks = 0;
    const Board board = pos->board;

    // initial row and column
    // N.B: (0, 0) is top left corner
    const int r0 = sq / 8, c0 = sq % 8;

    // up
    for (int r = r0 + 1; r <= 7; r++) {
        const Bitboard attackedSquare = 1 << xyToSquare(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // down
    for (int r = r0 - 1; r >= 0; r--) {
        const Bitboard attackedSquare = 1 << xyToSquare(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // left
    for (int c = c0 - 1; c >= 0; c--) {
        const Bitboard attackedSquare = 1 << xyToSquare(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    // right
    for (int c = c0 + 1; c <= 7; c++) {
        const Bitboard attackedSquare = 1 << xyToSquare(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & board.getOccupancy(BOTH_OCCUPANCY)) {
            break;
        }
    }

    return attacks;
}

constexpr Bitboard MoveGenerator::computeQueenAttacks(Square sq) {
    return computeBishopAttacks(sq) | computeRookAttacks(sq);  // heheh
}

constexpr Bitboard MoveGenerator::computeKingAttacks(Square sq) {
    const Bitboard sqBB = 1 << sq;
    const Bitboard d = (sqBB & not_rank_1) << 8;                // down
    const Bitboard u = (sqBB & not_rank_8) >> 8;                // up
    const Bitboard l = (sqBB & not_file_a) >> 1;                // left
    const Bitboard r = (sqBB & not_file_h) << 1;                // right
    const Bitboard dl = (sqBB & not_rank_1 & not_file_a) << 7;  // down left
    const Bitboard dr = (sqBB & not_rank_1 & not_file_h) << 9;  // down right
    const Bitboard ul = (sqBB & not_rank_8 & not_file_a) >> 9;  // up left
    const Bitboard ur = (sqBB & not_rank_8 & not_file_h) >> 7;  // up right

    return d | u | l | r | dl | dr | ul | ur;
}

/**
 * Returns return if and only if the given square is under attack by the given
 * side
 */
constexpr bool MoveGenerator::underAttack(Square sq, Color side) {
    // return (1 << sq) & (white ? (pieceAttacks[WP] | pieceAttacks[WN] | pieceAttacks[WB] |
    //                              pieceAttacks[WR] | pieceAttacks[WQ] | pieceAttacks[WK])
    //                           : (pieceAttacks[BP] | pieceAttacks[BN] | pieceAttacks[BB] |
    //                              pieceAttacks[BR] | pieceAttacks[BQ] | pieceAttacks[BK]));
    // TODO: This
    return false;
}

/**
 * Returns return if and only if the given square is under attack by the
 * opponent
 * @param
 * squareBit The square to check
 */
constexpr bool MoveGenerator::underAttack(Square sq) {
    const Color oppSide = Color((pos->getSideToMove() + 1) % 2);
    return underAttack(sq, oppSide);
}

constexpr bool MoveGenerator::insufficientMaterial() {
    // count all pieces
    auto board = pos->board;
    const int pieceCount[12] = {
        __builtin_popcountll(board.getPieces(WP)),
        __builtin_popcountll(board.getPieces(WN)),
        __builtin_popcountll(board.getPieces(WB)),
        __builtin_popcountll(board.getPieces(WR)),
        __builtin_popcountll(board.getPieces(WQ)),
        1,  // always 1 white king
        __builtin_popcountll(board.getPieces(BP)),
        __builtin_popcountll(board.getPieces(BN)),
        __builtin_popcountll(board.getPieces(BB)),
        __builtin_popcountll(board.getPieces(BR)),
        __builtin_popcountll(board.getPieces(BQ)),
        1  // always 1 black king
    };

    // note: consider testing for king + two knights vs king

    // insufficient material if at most one knight or bishop is left per side
    return !(pieceCount[WN] & pieceCount[WB]) & !(pieceCount[BN] & pieceCount[BB]) &
           !(pieceCount[WP] | pieceCount[WR] | pieceCount[WQ] | pieceCount[BP] | pieceCount[BR] |
             pieceCount[BQ]);
}

constexpr bool MoveGenerator::isTerminal() {
    // TODO: This
    return false;
}

constexpr bool MoveGenerator::isCheck() {
    // get position of king
    const Bitboard kingBB = pos->board.getPieces(pos->getSideToMove() == WHITE ? WK : BK);
    const Square kingSq = Square(indexOfLs1b(kingBB));

    // check if king is under attack
    return underAttack(kingSq);
}

constexpr bool MoveGenerator::isMoveLegal(Position &copy, Square kingSq, Move &move, Color side) {
    // simulate move on copied state
    Position::Metadata md = copy.makeMove(move);

    // test if king is in check after move
    bool isOurKingInCheck = underAttack(kingSq, Color((side + 1) % 2));

    // unmake move to preserve original state
    copy.unmakeMove(move, md);

    // return true iff the move is not illegal
    return !isOurKingInCheck;
}

const std::vector<Move> &MoveGenerator::generateLegal() {
    // TODO: This
    return {};
}

const std::vector<Move> &MoveGenerator::generatePseudolegal() {
    // TODO: This
    return {};
}
