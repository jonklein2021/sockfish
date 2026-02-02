#pragma once

#include "src/ai/Evaluator.h"
#include "src/ai/MoveSorter.h"
#include "src/ai/TranspositionTable.h"
#include "src/core/Move.h"
#include "src/core/Position.h"

class Searcher {
   private:
    TranspositionTable tt;

    MoveSorter moveSorter;

    Evaluator evaluator;

    // std::array<std::array<Move, MAX_PLY>, MAX_PLY> pvTable;

    // Temporarily stores the best move for easy access during negamax
    // TODO: Delete this after creating PV table
    Move bestMove;

    Eval negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth);

    Eval quiescenceSearch(Position &pos, Eval alpha, Eval beta, int ply);

   public:
    void stop();

    Move run(Position &pos, int maxDepth);
};
