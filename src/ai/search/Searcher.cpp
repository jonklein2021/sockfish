#include "Searcher.h"

#include "src/movegen/MoveGenerator.h"

#include <chrono>
#include <iostream>

Searcher::Searcher(SearchStopper *searchStopper)
    : searchStopper(searchStopper) {
    assert(searchStopper != nullptr);
}

void Searcher::setStopper(SearchStopper *searchStopper) {
    assert(searchStopper != nullptr);
    this->searchStopper = searchStopper;
}

void Searcher::addToRepetitionTable(uint64_t posHash) {
    repetitionTable.push(posHash);
}

void Searcher::clearRepetitionTable() {
    repetitionTable.clear();
}

void Searcher::abortSearch() {
    searchStopper->overrideAndAbort();
}

Eval Searcher::negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth) {
    // check for cancellation every 2048 nodes
    if ((nodesSearched & 2047) == 0 && searchStopper->isStopped()) {
        return 0;
    }

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

    // check for repetition
    if (ply > 0 && repetitionTable.contains(h)) {
        return 0;
    }

    // add this position to the repetition table temporarily
    repetitionTable.push(h);

    // increment node search count
    nodesSearched++;

    // save original alpha for TT record
    const Eval originalAlpha = alpha;

    // count legal moves to detect checkmate/stalemate after loop
    int legalMoveCount = 0;

    // get pseudolegal moves and sort them
    MoveList moves;
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

        Eval score = -negamax(pos, -beta, -alpha, ply + 1, depth - 1);

        pos.unmakeMove(move, md);

        // exit if time is up
        if (searchStopper->isStopped()) {
            repetitionTable.pop();
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
                tt.store(h, score, alpha, beta, ply, depth);
                repetitionTable.pop();
                return beta;
            }
        }
    }

    // erase this position from the repetition table
    repetitionTable.pop();

    // check for checkmate or stalemate
    if (legalMoveCount == 0) {
        // checkmate: add ply to favor faster mates
        if (pos.isCheck()) {
            return -MATE_SCORE + ply;
        }
        // stalemate: return 0 to indicate draw
        return 0;
    }

    // save result in transposition table
    tt.store(h, alpha, originalAlpha, beta, ply, depth);

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

    // check for cancellation every 2048 nodes
    if ((nodesSearched & 2047) == 0 && searchStopper->isStopped()) {
        return 0;
    }

    // check for repetition unless from root
    uint64_t h = pos.getHash();
    if (repetitionTable.contains(h)) {
        return 0;
    }

    // add this position to the repetition table temporarily
    repetitionTable.push(h);

    // increment node search count
    nodesSearched++;

    MoveList captureMoves;
    MoveGenerator::generatePseudolegalCaptures(captureMoves, pos);
    moveSorter.run(pos, captureMoves);

    // examine captures only
    for (const Move &move : captureMoves) {
        const Position::Metadata md = pos.makeMove(move);

        if (!pos.isLegal()) {
            pos.unmakeMove(move, md);
            continue;
        }

        const Eval score = -quiescenceSearch(pos, -beta, -alpha, ply + 1);

        pos.unmakeMove(move, md);

        // exit if time is up
        if (searchStopper->isStopped()) {
            repetitionTable.pop();
            return 0;
        }

        // found a better move
        if (score > alpha) {
            alpha = score;

            // node fails high
            if (score >= beta) {
                repetitionTable.pop();
                return beta;
            }
        }
    }

    repetitionTable.pop();

    // node fails low
    return alpha;
}

// We make pos a copy to prevent the worker thread from leaving the
// global pos reference in an invalid state
Move Searcher::run(Position pos, int maxDepth) {
    searchStopper->reset();

    // N.B: Remember to clear helper DSs here (killer moves, PV table, etc)
    nodesSearched = 0;
    bestMove = Move::none();

    // these variables are only updated for full searches
    Move bestFullySearchedMove = Move::none();
    Eval bestFullySearchedEval = -INFINITY;

    // for reporting NPS
    auto start = std::chrono::steady_clock::now();

    // iterative deepeninuug
    for (int depth = 1; depth <= maxDepth; depth++) {
        // TODO: ensure PV move is examined first
        Eval score = negamax(pos, -INFINITY, INFINITY, 0, depth);
        auto end = std::chrono::steady_clock::now();
        int duration_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // exit early if search was cancelled
        if (searchStopper->isStopped()) {
            break;
        }

        // otherwise, update the top move
        bestFullySearchedMove = bestMove;
        bestFullySearchedEval = score;

        // UCI info out
        std::cout << "info depth " << depth << " ";
        std::cout << "nodes " << nodesSearched << " ";
        std::cout << "score cp " << bestFullySearchedEval << std::endl;

        if (duration_ms > 1000) {
            std::cout << "info nps " << 1000 * nodesSearched / duration_ms << std::endl;
        }
    }

    return bestFullySearchedMove;
}
