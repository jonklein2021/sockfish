#include "Engine.h"

#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <cstdio>
#include <iostream>
#include <limits>

Engine::Engine()
    : maxDepth(4) {}

Engine::Engine(int depth)
    : maxDepth(depth) {}

Eval Engine::negamax(Position &pos, Eval alpha, Eval beta, int depth) {
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
        return quiescenceSearch(pos, alpha, beta);
    }

    // get legal moves and sort them
    std::vector<Move> legalMoves;
    MoveGenerator::generateLegal(legalMoves, pos);
    moveSorter.run(pos, legalMoves);

    // save original alpha for TT record
    Eval alphaOrig = alpha;
    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos.makeMove(move);

        Eval score = -negamax(pos, -beta, -alpha, depth - 1);

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

    // check for checkmate or stalemate
    if (legalMoves.empty()) {
        // checkmate: return -500000 + ply to favor faster mates
        if (PositionUtil::isCheck(pos)) {
            return -CHECKMATE_EVAL - depth;
        }
        // stalemate: return 0 to indicate draw
        return 0;
    }

    // save result in transposition table
    tt.store(h, alpha, alphaOrig, beta, depth);

    // node fails low
    return alpha;
}

Eval Engine::quiescenceSearch(Position &pos, Eval alpha, Eval beta) {
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

        Eval score = -quiescenceSearch(pos, -beta, -alpha);

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
    std::vector<Move> legalMoves;
    MoveGenerator::generateLegal(legalMoves, pos);
    return getMove(pos, legalMoves);
}

Move Engine::getMove(Position &pos, std::vector<Move> &legalMoves) {
    if (legalMoves.empty()) {
        return Move::none();
    }

    // sort moves by MVV-LVA
    moveSorter.run(pos, legalMoves);

    std::cout << "Analyzing moves..." << std::endl;

    Move bestMove = Move::none();
    Eval bestEval = std::numeric_limits<Eval>::lowest();

    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos.makeMove(move);

        std::cout << "  " << move.toString() << std::endl;

        Eval eval = -negamax(pos, -pieceTypeValues[KING], pieceTypeValues[KING], maxDepth);

        std::cout << "\teval = " << eval << std::endl;
        pos.unmakeMove(move, md);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
