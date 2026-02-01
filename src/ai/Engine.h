#pragma once

#include "src/ai/Evaluator.h"
#include "src/ai/MoveSorter.h"
#include "src/ai/PolyglotBook.h"
#include "src/ai/TranspositionTable.h"
#include "src/core/Position.h"

class Engine {
   private:
    /**
     * The maximum depth to search
     *
     * TODO: Increase this a lot after creating an interrupt mechanism
     */
    static constexpr int MAX_PLY = 6;

    TranspositionTable tt;

    MoveSorter moveSorter;

    Evaluator evaluator;

    PolyglotBook openingBook;

    // std::array<std::array<Move, MAX_PLY>, MAX_PLY> pvTable;

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
