#include "Engine.h"

#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <cstdio>

Eval Engine::negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth) {
    uint64_t h = pos.getHash();

    // base case: TT has evaluation
    if (const auto e = tt.lookup(h, depth)) {
        if (e->flag == EXACT) {
            return e->eval;
        }
        if (e->flag == LOWERBOUND && e->eval >= beta) {
            return e->eval;
        }
        if (e->flag == UPPERBOUND && e->eval <= alpha) {
            return e->eval;
        }
    }

    // base case: depth exceeded
    // NTS: what happens if it is checkmate at depth 0?
    if (depth == 0) {
        return quiescenceSearch(pos, alpha, beta, ply);
    }

    // get legal moves and sort them
    std::vector<Move> legalMoves;
    MoveGenerator::generateLegal(legalMoves, pos);
    moveSorter.run(pos, legalMoves);

    // save original alpha for TT record
    Eval originalAlpha = alpha;
    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos.makeMove(move);

        Eval score = -negamax(pos, -beta, -alpha, ply + 1, depth - 1);

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
    if (legalMoves.empty()) {
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
    MoveGenerator::generateLegalCaptures(captureMoves, pos);
    moveSorter.run(pos, captureMoves);

    // examine every capture
    for (const Move &move : captureMoves) {
        const Position::Metadata md = pos.makeMove(move);

        Eval score = -quiescenceSearch(pos, -beta, -alpha, ply + 1);

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
