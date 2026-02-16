#include "PositionUtil.h"

#include "src/bitboard/bit_tools.h"
#include "src/movegen/MoveGenerator.h"

bool PositionUtil::insufficientMaterial(Position &pos) {
    // count all pieces
    const int pieceCount[12] = {
        getBitCount(pos.getPieceBB(WP)),
        getBitCount(pos.getPieceBB(WN)),
        getBitCount(pos.getPieceBB(WB)),
        getBitCount(pos.getPieceBB(WR)),
        getBitCount(pos.getPieceBB(WQ)),
        1,  // always 1 white king
        getBitCount(pos.getPieceBB(BP)),
        getBitCount(pos.getPieceBB(BN)),
        getBitCount(pos.getPieceBB(BB)),
        getBitCount(pos.getPieceBB(BR)),
        getBitCount(pos.getPieceBB(BQ)),
        1  // always 1 black king
    };

    // note: consider testing for king + two knights vs king

    // insufficient material if at most one knight or bishop is left per side
    return !(pieceCount[WN] & pieceCount[WB]) & !(pieceCount[BN] & pieceCount[BB]) &
           !(pieceCount[WP] | pieceCount[WR] | pieceCount[WQ] | pieceCount[BP] | pieceCount[BR] |
             pieceCount[BQ]);
}

bool PositionUtil::is50MoveRuleDraw(const Position &pos) {
    return pos.md.movesSinceCapture >= 100;  // 50 moves for EACH PLAYER
}

// TODO: Add more conditions to ensure this doesn't run for checkmate situations
bool PositionUtil::isStalemate(const std::vector<Move> &legalMoves) {
    return legalMoves.empty();
}

bool PositionUtil::isStalemate(Position &pos) {
    std::vector<Move> moves;
    MoveGenerator::generateLegal(moves, pos);
    return isStalemate(moves);
}

bool PositionUtil::isCheckmate(Position &pos) {
    return pos.isCheck() && isStalemate(pos);
}

bool PositionUtil::isTerminal(Position &pos) {
    // N.B. isStalemate covers checkmate cases as well
    return isStalemate(pos) || insufficientMaterial(pos) || is50MoveRuleDraw(pos);
}

// TODO: Handle threefold repetition
GameStatus PositionUtil::getGameStatus(Position &pos) {
    if (isCheckmate(pos)) {
        return CHECKMATE;
    } else if (isStalemate(pos)) {
        return DRAW_BY_STALEMATE;
    } else if (insufficientMaterial(pos)) {
        return DRAW_BY_INSUFFICIENT_MATERIAL;
    } else if (is50MoveRuleDraw(pos)) {
        return DRAW_BY_50_MOVE_RULE;
    } else {
        return IN_PROGRESS;
    }
}
