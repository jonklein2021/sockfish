#include "Searcher.h"

#include "src/core/Notation.h"
#include "src/movegen/MoveGenerator.h"

#include <cstdio>

Searcher::Searcher(SearchStopper &searchStopper)
    : searchStopper(searchStopper) {}

Eval Searcher::negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth) {
    // uint64_t h = pos.getHash();

    // check for a TT entry
    // if (ply > 0) {
    //     TTEntry tte = tt.lookup(h, depth);
    //     if (tte.flag != TTFlag::NO_ENTRY) {
    //         return tte.eval;
    //     }
    // }

    // every 2048 nodes, check for cancellation
    if ((nodesSearched & 2047) == 0 && searchStopper.isStopped()) {
        return 0;
    }

    // base case: depth exceeded
    // NTS: what happens if it is checkmate at depth 0?
    if (depth == 0) {
        return quiescenceSearch(pos, alpha, beta, ply);
    }

    // increment node search count
    nodesSearched++;

    // save original alpha for TT record
    // const Eval originalAlpha = alpha;

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

        // exit if time is up
        if (searchStopper.isStopped()) {
            return 0;
        }

        // found a better move
        if (score > alpha) {
            alpha = score;
            // update bestMove only if from root
            if (ply == 0) {
                bestMove = move;
            }

            // node fails high
            if (score >= beta) {
                // tt.store(h, score, alpha, beta, depth);
                return beta;
            }
        }
    }

    // check for checkmate or stalemate
    if (legalMoveCount == 0) {
        // checkmate: return -500000 + ply to favor faster mates
        if (PositionUtil::isCheck(pos)) {
            return -INFINITY + ply;
        }
        // stalemate: return 0 to indicate draw
        return 0;
    }

    // save result in transposition table
    // tt.store(h, alpha, originalAlpha, beta, depth);

    // node fails low
    return alpha;
}

Eval Searcher::quiescenceSearch(Position &pos, Eval alpha, Eval beta, int ply) {
    // initialize with static eval
    Eval staticEval = evaluator.run(pos);
    if (staticEval >= beta) {
        return staticEval;
    }
    if (staticEval > alpha) {
        alpha = staticEval;
    }

    // every 2048 nodes, check for cancellation
    if ((nodesSearched & 2047) == 0 && searchStopper.isStopped()) {
        return 0;
    }

    // increment node search count
    nodesSearched++;

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

// We make pos a copy to prevent the worker thread from leaving the
// global pos reference in an invalid state
Move Searcher::run(Position pos, int maxDepth) {
    // N.B: Remember to clear helper DSs here (killer moves, PV table, etc)
    searchStopper.reset();
    nodesSearched = 0;
    bestMove = Move::none();

    // these variables are only updated for full searches
    Move bestFullySearchedMove = Move::none();
    Eval bestFullySearchedEval = -INFINITY;

    // iterative deepening
    for (int depth = 1; depth <= maxDepth; depth++) {
        // TODO: ensure PV move is examined first
        Eval score = negamax(pos, -INFINITY, INFINITY, 0, depth);

        if (searchStopper.isStopped()) {
            // exit early if search was cancelled
            break;
        } else {
            // otherwise, update the top move
            bestFullySearchedMove = bestMove;
            bestFullySearchedEval = score;

            printf("info depth %d bestmove %s score cp %d\n", depth,
                   Notation::moveToUci(bestFullySearchedMove).c_str(), bestFullySearchedEval);
        }
    }

    return bestFullySearchedMove;
}
