#pragma once

#include "Move.h"
#include "Position.h"
#include "bit_tools.h"

#include <memory>
#include <vector>

// TODO: Make this Stateless

class MoveGenerator {
   private:
    std::shared_ptr<Position> pos;

    Bitboard computeAllSidesAttacks();
    Bitboard computeAllSidesAttacks(Color color);
    Bitboard computePieceAttacks(Piece piece);

    /**
     * Returns return if and only if the given square is under attack by the given
     * side
     */
    bool underAttack(Square sq, Color side) {
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
    bool underAttack(Square sq) {
        return underAttack(sq, otherColor(pos->getSideToMove()));
    }

    bool insufficientMaterial() {
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
               !(pieceCount[WP] | pieceCount[WR] | pieceCount[WQ] | pieceCount[BP] |
                 pieceCount[BR] | pieceCount[BQ]);
    }

    bool isTerminal() {
        // TODO: This
        return false;
    }

    bool isCheck() {
        // get position of king
        const Bitboard kingBB = pos->board.getPieces(pos->getSideToMove() == WHITE ? WK : BK);
        const Square kingSq = Square(indexOfLs1b(kingBB));

        // check if king is under attack
        return underAttack(kingSq);
    }

    bool isMoveLegal(Position &copy, Square kingSq, Move &move, Color side) {
        // simulate move on copied state
        Position::Metadata md = copy.makeMove(move);

        // test if king is in check after move
        bool isOurKingInCheck = underAttack(kingSq, otherColor(side));

        // unmake move to preserve original state
        copy.unmakeMove(move, md);

        // return true iff the move is not illegal
        return !isOurKingInCheck;
    }

    void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq);

    template<typename MoveComputer>
    void appendMovesFromPiece(std::vector<Move> &moveList, PieceType pt, MoveComputer moveComputer);

   public:
    MoveGenerator(std::shared_ptr<Position> pos);

    std::vector<Move> generateLegal();
    std::vector<Move> generatePseudolegal();
};
