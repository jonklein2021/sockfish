#pragma once

#include "src/ai/Evaluator.h"
#include "src/ai/MoveSorter.h"
#include "src/ai/TranspositionTable.h"
#include "src/core/Move.h"
#include "src/core/Position.h"

#include <atomic>

class Searcher {
   private:
    TranspositionTable tt;

    MoveSorter moveSorter;

    Evaluator evaluator;

    // std::array<std::array<Move, MAX_PLY>, MAX_PLY> pvTable;

    // Temporarily stores the best move for easy access during negamax
    // TODO: Delete this after creating PV table
    Move bestMove;

    std::atomic<bool> *searchCancelled;

    Eval negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth);

    Eval quiescenceSearch(Position &pos, Eval alpha, Eval beta, int ply);

    void findBestMove(Position &pos, int maxDepth);

    bool isSearchCancelled();

   public:
    void setStopFlagPtr(std::atomic<bool> *stopFlag) {
        this->searchCancelled = stopFlag;
    }

    Move run(Position pos, int maxDepth);
};
