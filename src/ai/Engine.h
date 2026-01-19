#pragma once

#include "src/ai/Evaluator.h"
#include "src/ai/MoveSorter.h"
#include "src/ai/TranspositionTable.h"
#include "src/core/Position.h"

class Engine {
   private:
    Evaluator evaluator;

    TranspositionTable tt;

    MoveSorter moveSorter;

    /**
     * The maximum depth to search
     *
     * TODO: Increase this a lot after creating an interrupt mechanism
     */
    const int MAX_PLY = 6;

    // Temporarily stores the best move for easy access during negamax
    // TODO: Delete this after creating PV table
    Move bestMove;

    /**
     * Minimax variant with alpha-beta pruning
     *
     * @param pos to evaluate
     * @param alpha The alpha value
     * @param beta The beta value
     * @return The heuristic value of the position to move to
     */
    Eval negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth);

    Eval quiescenceSearch(Position &pos, Eval alpha, Eval beta, int ply);

   public:
    Move getMove(Position &pos);

    /**
     * Gets the best move in a certain position using iterative deepening
     *
     * @param pos to evaluate
     * @param maxDepth to search to
     * @return The best move to make
     */
    Move getMove(Position &pos, int maxDepth);
};
