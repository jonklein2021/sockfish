#include "Engine.h"

#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <cstdio>

Eval Engine::negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth) {
    uint64_t h = pos.getHash();

    // check for a TT entry
    if (ply > 0) {
        TTEntry tte = tt.lookup(h, depth);
        if (tte.flag != TTFlag::NO_ENTRY) {
            return tte.eval;
        }
    }

    // base case: depth exceeded
    // NTS: what happens if it is checkmate at depth 0?
    if (depth == 0) {
        return quiescenceSearch(pos, alpha, beta, ply);
    }

    // save original alpha for TT record
    const Eval originalAlpha = alpha;

    // count legal moves to detect checkmate/stalemate after loop
    int legalMoveCount = 0;

    // get pseudolegal moves and sort them
    std::vector<Move> moves;
    MoveGenerator::generatePseudolegal(moves, pos);
    moveSorter.run(pos, moves);

    for (const Move &move : moves) {
        const Position::Metadata md = pos.makeMove(move);

        // move wasn't legal; undo it and skip the rest of this loop
        if (!pos.isLegal()) {
            pos.unmakeMove(move, md);
            continue;
        }

        legalMoveCount++;

        const Eval score = -negamax(pos, -beta, -alpha, ply + 1, depth - 1);

        pos.unmakeMove(move, md);

        // node fails high
        if (score >= beta) {
            return beta;
        }

        // found a better move
        if (score > alpha) {
            alpha = score;
            // update bestMove only if from root
            if (ply == 0) {
                bestMove = move;
            }
        }
    }

    // check for checkmate or stalemate
    if (legalMoveCount == 0) {
        // checkmate: return -500000 + ply to favor faster mates
        if (PositionUtil::isCheck(pos)) {
            return -CHECKMATE_EVAL + ply;
        }
        // stalemate: return 0 to indicate draw
        return 0;
    }

    // save result in transposition table
    tt.store(h, alpha, originalAlpha, beta, depth);

    // node fails low
    return alpha;
}

Eval Engine::quiescenceSearch(Position &pos, Eval alpha, Eval beta, int ply) {
    // initialize with static eval
    Eval staticEval = evaluator.run(pos);
    if (staticEval >= beta) {
        return staticEval;
    }
    if (staticEval > alpha) {
        alpha = staticEval;
    }

    std::vector<Move> captureMoves;
    MoveGenerator::generatePseudolegalCaptures(captureMoves, pos);
    moveSorter.run(pos, captureMoves);

    // examine every capture
    for (const Move &move : captureMoves) {
        const Position::Metadata md = pos.makeMove(move);

        if (!pos.isLegal()) {
            pos.unmakeMove(move, md);
            continue;
        }

        const Eval score = -quiescenceSearch(pos, -beta, -alpha, ply + 1);

        pos.unmakeMove(move, md);

        // node fails high
        if (score >= beta) {
            return beta;
        }

        // found a better move
        if (score > alpha) {
            alpha = score;
        }
    }

    // node fails low
    return alpha;
}

Move Engine::getMove(Position &pos) {
    return getMove(pos, MAX_PLY);
}

Move Engine::getMove(Position &pos, int maxDepth) {
    // N.B: Remember to clear helper DSs here (killer moves, PV table, etc)
    bestMove = Move::none();

    const Eval initAlpha = -CHECKMATE_EVAL, initBeta = CHECKMATE_EVAL;
    for (int depth = 1; depth <= maxDepth; depth++) {
        // TODO: ensure PV move is examined first
        negamax(pos, initAlpha, initBeta, 0, depth);
    }

    return bestMove;
}
