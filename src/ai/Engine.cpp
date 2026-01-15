#include "Engine.h"

#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <limits>

Engine::Engine()
    : maxDepth(4) {}

Engine::Engine(int depth)
    : maxDepth(depth) {}

Eval Engine::negamax(Position &pos, Eval alpha, Eval beta, int depth) {
    Eval alphaOrig = alpha;
    uint64_t h = pos.getHash();
    int remainingDepth = maxDepth - depth;
    if (transpositionTable.find(h) != transpositionTable.end()) {
        const TTEntry &e = transpositionTable[h];
        if (e.depth >= remainingDepth) {
            if (e.flag == TTEntry::EXACT) {
                return e.eval;
            }
            if (e.flag == TTEntry::LOWERBOUND) {
                alpha = std::max(alpha, e.eval);
            }
            if (e.flag == TTEntry::UPPERBOUND) {
                beta = std::max(beta, e.eval);
            }
            if (alpha >= beta) {
                return e.eval;
            }
        }
    }

    // base case
    if (depth >= maxDepth || PositionUtil::isTerminal(pos)) {
        return quiescenceSearch(pos, alpha, beta);
    }

    std::vector<Move> legalMoves;
    MoveGenerator::generateLegal(legalMoves, pos);
    moveSorter.run(pos, legalMoves);

    Eval bestEval = std::numeric_limits<Eval>::lowest();

    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos.makeMove(move);

        Eval eval = -negamax(pos, -beta, -alpha, depth + 1);

        pos.unmakeMove(move, md);

        bestEval = std::max(bestEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }

    // save result in transposition table
    TTEntry entry;
    entry.eval = bestEval;
    entry.depth = remainingDepth;
    if (bestEval <= alphaOrig) {
        entry.flag = TTEntry::UPPERBOUND;
    } else if (bestEval >= beta) {
        entry.flag = TTEntry::LOWERBOUND;
    } else {
        entry.flag = TTEntry::EXACT;
    }
    transpositionTable[h] = entry;

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

        Eval eval = -negamax(pos, -pieceTypeValues[KING], pieceTypeValues[KING], 0);

        std::cout << "\teval = " << eval << std::endl;
        pos.unmakeMove(move, md);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
