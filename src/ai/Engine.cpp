#include "Engine.h"

#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <algorithm>
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
    // N.B: even if this returns false, alpha and beta may be updated from the TT
    if (const auto eval = tt.lookup(h, alpha, beta, depth)) {
        return eval.value();
    }

    // base case: depth exceeded or terminal position
    if (depth == 0 || PositionUtil::isTerminal(pos)) {
        return quiescenceSearch(pos, alpha, beta);
    }

    Eval alphaOrig = alpha;

    std::vector<Move> legalMoves;
    MoveGenerator::generateLegal(legalMoves, pos);
    moveSorter.run(pos, legalMoves);

    Eval bestEval = std::numeric_limits<Eval>::lowest();

    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos.makeMove(move);

        Eval eval = -negamax(pos, -beta, -alpha, depth - 1);

        pos.unmakeMove(move, md);

        bestEval = std::max(bestEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }

    // save result in transposition table
    tt.store(h, bestEval, alphaOrig, beta, depth);

    return bestEval;
}

Eval Engine::quiescenceSearch(Position &pos, Eval alpha, Eval beta) {
    // initialize with static eval
    int bestEval = evaluator.run(pos);
    if (bestEval >= beta) {
        return bestEval;
    }
    if (bestEval > alpha) {
        alpha = bestEval;
    }

    std::vector<Move> captureMoves;
    MoveGenerator::generateLegalCaptures(captureMoves, pos);
    moveSorter.run(pos, captureMoves);

    // examine every capture
    for (const Move &move : captureMoves) {
        const Position::Metadata md = pos.makeMove(move);

        Eval eval = -quiescenceSearch(pos, -beta, -alpha);

        pos.unmakeMove(move, md);

        bestEval = std::max(bestEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }

    return bestEval;
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

    Eval bestEval = std::numeric_limits<Eval>::lowest();
    Move bestMove;

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
