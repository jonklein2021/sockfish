#include "PositionUtil.h"

#include "src/bitboard/bit_tools.h"
#include "src/movegen/MoveGenerator.h"

bool PositionUtil::isCheck(Position &pos, Color defender) {
    const Bitboard kingBB = pos.getPieceBB(ptToPiece(KING, defender));
    const Square kingSq = Square(getLsbIndex(kingBB));

    return pos.isAttacked(kingSq, otherColor(defender));
}

bool PositionUtil::isCheck(Position &pos) {
    return isCheck(pos, pos.getSideToMove());
}

bool PositionUtil::insufficientMaterial(Position &pos) {
    // count all pieces
    const int pieceCount[12] = {
        __builtin_popcountll(pos.getPieceBB(WP)),
        __builtin_popcountll(pos.getPieceBB(WN)),
        __builtin_popcountll(pos.getPieceBB(WB)),
        __builtin_popcountll(pos.getPieceBB(WR)),
        __builtin_popcountll(pos.getPieceBB(WQ)),
        1,  // always 1 white king
        __builtin_popcountll(pos.getPieceBB(BP)),
        __builtin_popcountll(pos.getPieceBB(BN)),
        __builtin_popcountll(pos.getPieceBB(BB)),
        __builtin_popcountll(pos.getPieceBB(BR)),
        __builtin_popcountll(pos.getPieceBB(BQ)),
        1  // always 1 black king
    };

    // note: consider testing for king + two knights vs king

    // insufficient material if at most one knight or bishop is left per side
    return !(pieceCount[WN] & pieceCount[WB]) & !(pieceCount[BN] & pieceCount[BB]) &
           !(pieceCount[WP] | pieceCount[WR] | pieceCount[WQ] | pieceCount[BP] | pieceCount[BR] |
             pieceCount[BQ]);
}

bool PositionUtil::isCheckmate(Position &pos) {
    return isCheck(pos) && MoveGenerator::generateLegal(pos).empty();
}

bool PositionUtil::isTerminal(Position &pos) {
    return insufficientMaterial(pos) || isCheckmate(pos);
}
