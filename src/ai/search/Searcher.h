#pragma once

#include "src/ai/Evaluator.h"
#include "src/ai/MoveSorter.h"
#include "src/ai/TranspositionTable.h"
#include "src/ai/search/SearchStopper.h"
#include "src/core/Move.h"
#include "src/core/Position.h"

#include <unordered_set>

class Searcher {
   private:
    // TranspositionTable tt;

    MoveSorter moveSorter;

    Evaluator evaluator;

    SearchStopper *searchStopper;

    // std::array<std::array<Move, MAX_PLY>, MAX_PLY> pvTable;

    // TODO: replace this with a vector or array
    std::unordered_set<uint64_t> repetitionTable;

    uint64_t nodesSearched;

    // Temporarily stores the best move for easy access during negamax
    // TODO: Delete this after creating PV table
    Move bestMove;

    Eval negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth);

    Eval quiescenceSearch(Position &pos, Eval alpha, Eval beta, int ply);

   public:
    Searcher(SearchStopper *searchStopper);

    void setStopper(SearchStopper *searchStopper);

    void addToRepetitionTable(uint64_t posHash);

    void abortSearch();

    Move run(Position pos, int maxDepth);
};
