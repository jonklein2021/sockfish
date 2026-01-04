#include "MoveGenerator.h"

#include "Move.h"
#include "MoveComputers.h"
#include "Position.h"
#include "bit_tools.h"
#include "src/types.h"

#include <algorithm>

bool MoveGenerator::isMoveLegal(std::shared_ptr<Position> pos, Move &move) {
    Color moveMaker = pos->getSideToMove();

    // simulate move on copied state
    Position::Metadata md = pos->makeMove(move);

    // test if king is in check after move
    bool isOurKingInCheck = PositionUtil::isCheck(pos, moveMaker);

    // unmake move to preserve original state
    pos->unmakeMove(move, md);

    // return true iff the move hasn't put its own king in check
    return !isOurKingInCheck;
}

void appendPawnMoves(std::vector<Move> &moveList, std::shared_ptr<Position> pos) {
    static constexpr Direction DIR[2] = {NORTH, SOUTH};

    // double pawn pushes must land on these ranks
    static constexpr Bitboard DBL[2] = {rank4, rank5};

    static constexpr int ATTACKS[2][2] = {
        {NORTH_EAST, NORTH_WEST},  // white
        {SOUTH_EAST, SOUTH_WEST}   // black
    };

    static constexpr Bitboard FILE_MASKS[2] = {
        not_file_a,  // east moves
        not_file_h   // west moves
    };

    const Color side = pos->getSideToMove();
    const Bitboard oppPieces = pos->getBoard().getOccupancy(otherColor(side));
    const Bitboard emptySquares = pos->getBoard().getEmptySquares();
    const Bitboard epSqBB = (1ull << pos->md.enPassantSquare);

    Bitboard pawnBB = pos->getPieceBB(ptToPiece(PAWN, side));

    while (pawnBB) {
        Square srcSq = Square(getLsbIndex(pawnBB));

        // destination square must be empty
        const Square singlePushDstSq = Square(srcSq + DIR[side]);

        // ensure pawn isn't blocked
        if ((1ull << singlePushDstSq) & emptySquares) {
            // check for promotion
            if (singlePushDstSq <= h8 || singlePushDstSq >= a1) {
                for (PieceType pt : PROMOTION_PIECE_TYPES) {
                    moveList.emplace_back(
                        Move::create<Move::PROMOTION>(srcSq, singlePushDstSq, pt));
                }
            } else {
                moveList.emplace_back(srcSq, singlePushDstSq);
            }
        }

        // double pawn push requires pawn to land on the 4th rank
        // AND 2 empty squares in front of it
        const Square doublePushDstSq = Square(singlePushDstSq + DIR[side]);
        if ((1ull << doublePushDstSq) & emptySquares & DBL[side]) {
            moveList.emplace_back(srcSq, doublePushDstSq);
        }

        const Square eastCaptureSq = Square(srcSq + ATTACKS[side][0]);
        const Square westCaptureSq = Square(srcSq + ATTACKS[side][1]);

        if ((1ull << eastCaptureSq) & FILE_MASKS[0] & oppPieces) {
            if (eastCaptureSq <= h8 || eastCaptureSq >= a1) {
                for (PieceType pt : PROMOTION_PIECE_TYPES) {
                    moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, eastCaptureSq, pt));
                }
            } else {
                moveList.emplace_back(srcSq, eastCaptureSq);
            }
        }

        if ((1ull << westCaptureSq) & FILE_MASKS[1] & oppPieces) {
            if (westCaptureSq <= h8 || westCaptureSq >= a1) {
                for (PieceType pt : PROMOTION_PIECE_TYPES) {
                    moveList.emplace_back(Move::create<Move::PROMOTION>(srcSq, westCaptureSq, pt));
                }
            } else {
                moveList.emplace_back(srcSq, westCaptureSq);
            }
        }

        if ((1ull << eastCaptureSq) & FILE_MASKS[0] & epSqBB) {
            moveList.emplace_back(Move::create<Move::EN_PASSANT>(srcSq, eastCaptureSq));
        }

        if ((1ull << westCaptureSq) & FILE_MASKS[1] & epSqBB) {
            moveList.emplace_back(Move::create<Move::EN_PASSANT>(srcSq, westCaptureSq));
        }

        pawnBB &= pawnBB - 1;
    }
}

// only used for normal moves
void MoveGenerator::appendMovesFromBitboard(std::vector<Move> &moveList,
                                            Bitboard moves,
                                            Square srcSq) {
    while (moves) {
        const Square destSq = Square(getLsbIndex(moves));
        moveList.emplace_back(srcSq, destSq);
        moves &= moves - 1;
    }
}

template<PieceType pt, typename MoveComputer>
void MoveGenerator::appendMovesFromPiece(std::shared_ptr<Position> pos,
                                         std::vector<Move> &moveList,
                                         MoveComputer moveComputer) {
    const Color toMove = pos->getSideToMove();
    const Piece piece = ptToPiece(pt, toMove);
    Bitboard bb = pos->getPieceBB(piece);  // N.B: this needs to be a copy
    while (bb) {
        const Square srcSq = Square(getLsbIndex(bb));

        // compute a bitboard of all destination squares that this piece can go to,
        // according to the specified moveComputer
        Bitboard computedDstBB = moveComputer(pos, srcSq);

        // create and append moves to the moveList
        appendMovesFromBitboard(moveList, computedDstBB, srcSq);

        // pop this bit
        bb &= bb - 1;
    }
}

Move MoveGenerator::createCastlingMove(bool isQueenside, Color side) {
    // {kingside: {white, black}, queenside: {white, black}}
    static constexpr Square ROOK_FROM[2][2] = {{h1, h8}, {a1, a8}};
    static constexpr Square ROOK_TO[2][2] = {{f1, f8}, {d1, d8}};

    return Move::create<Move::CASTLING>(ROOK_FROM[isQueenside][side], ROOK_TO[isQueenside][side]);
}

void MoveGenerator::appendCastlingMoves(std::vector<Move> &moveList,
                                        std::shared_ptr<Position> pos) {
    // for each of the following constexpr arrays, the 1st element is relevant for white, and the
    // 2nd for black. this helps avoid branching

    static constexpr std::array<CastleRights, 2> KINGSIDE = {WHITE_OO, BLACK_OO};
    static constexpr std::array<CastleRights, 2> QUEENSIDE = {WHITE_OOO, BLACK_OOO};

    static constexpr Bitboard EMPTY_K[2] = {(1ull << f1) | (1ull << g1),
                                            (1ull << f8) | (1ull << g8)};

    static constexpr Bitboard EMPTY_Q[2] = {(1ull << d1) | (1ull << c1) | (1ull << b1),
                                            (1ull << d8) | (1ull << c8) | (1ull << b8)};

    static constexpr Square PASS_K[2][2] = {{f1, g1}, {f8, g8}};

    static constexpr Square PASS_Q[2][2] = {{d1, c1}, {d8, c8}};

    const Color side = pos->getSideToMove();
    const CastleRights cr = pos->getMetadata().castleRights;
    const Bitboard empty = pos->getBoard().getEmptySquares();

    // cannot castling out of check
    if (PositionUtil::isCheck(pos, side)) {
        return;
    }

    // --- Kingside ---
    if ((cr & KINGSIDE[side]) && (empty & EMPTY_K[side]) == EMPTY_K[side] &&
        !PositionUtil::isUnderAttack(pos, PASS_K[side][0], otherColor(side)) &&
        !PositionUtil::isUnderAttack(pos, PASS_K[side][1], otherColor(side))) {

        moveList.push_back(createCastlingMove(false, side));
    }

    // --- Queenside ---
    if ((cr & QUEENSIDE[side]) && (empty & EMPTY_Q[side]) == EMPTY_Q[side] &&
        !PositionUtil::isUnderAttack(pos, PASS_Q[side][0], otherColor(side)) &&
        !PositionUtil::isUnderAttack(pos, PASS_Q[side][1], otherColor(side))) {

        moveList.push_back(createCastlingMove(true, side));
    }
}

// TODO: Finish this
std::vector<Move> MoveGenerator::generatePseudolegal(std::shared_ptr<Position> pos) {
    // cannot do 218 max move space optimization since these are pseudolegal moves
    std::vector<Move> moveList;

    /* PAWNS */
    appendPawnMoves(moveList, pos);

    /* KNIGHTS */
    appendMovesFromPiece<KNIGHT>(pos, moveList, MoveComputers::computeKnightMoves);

    // TODO: Speed up sliding piece move gen with magic numbers

    /* BISHOPS */
    appendMovesFromPiece<BISHOP>(pos, moveList, MoveComputers::computeBishopMoves);

    /* ROOKS */
    appendMovesFromPiece<ROOK>(pos, moveList, MoveComputers::computeRookMoves);

    /* QUEENS */
    appendMovesFromPiece<QUEEN>(pos, moveList, MoveComputers::computeQueenMoves);

    /* KING */
    appendMovesFromPiece<KING>(pos, moveList, MoveComputers::computeKingMoves);

    /* CASTLING */
    appendCastlingMoves(moveList, pos);

    return moveList;
}

std::vector<Move> MoveGenerator::generateLegal(std::shared_ptr<Position> pos) {
    std::vector<Move> pseudolegal = generatePseudolegal(pos);
    std::vector<Move> legal;
    legal.reserve(218);

    // only copy non-violating moves to legal vector
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(legal),
                 [&](Move m) { return isMoveLegal(pos, m); });

    return legal;
}
