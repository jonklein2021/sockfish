#pragma once

#include "src/ai/Evaluator.h"
#include "src/ai/MoveSorter.h"
#include "src/ai/TranspositionTable.h"
#include "src/core/Position.h"

#include <vector>

class Engine {
   private:
    TranspositionTable tt;

    MoveSorter moveSorter;

    Evaluator evaluator;

    /**
     * The maximum depth to search
     */
    int maxDepth;

    /**
     * Minimax variant with alpha-beta pruning
     *
     * @param pos to evaluate
     * @param alpha The alpha value
     * @param beta The beta value
     * @return The heuristic value of the position to move to
     */
    Eval negamax(Position &pos, Eval alpha, Eval beta, int depth);

    Eval quiescenceSearch(Position &pos, Eval alpha, Eval beta);

   public:
    Engine();

    Engine(int depth);

    constexpr void setDepth(int depth) {
        maxDepth = depth;
    }

    Move getMove(Position &pos);

    /**
     * Gets the best move in a certain position
     *
     * @param pos to evaluate
     * @param legalMoves The possible legal moves from that position
     * @return The best move to make
     */
    Move getMove(Position &pos, std::vector<Move> &legalMoves);
};
