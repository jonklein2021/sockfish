#pragma once

#include "src/ai/Evaluator.h"
#include "src/ai/MoveSorter.h"
#include "src/ai/PVTable.h"
#include "src/ai/RepetitionTable.h"
#include "src/ai/TranspositionTable.h"
#include "src/ai/search/SearchStopper.h"
#include "src/core/Move.h"
#include "src/core/Position.h"

class Searcher {
   private:
    TranspositionTable tt;

    MoveSorter moveSorter;

    Evaluator evaluator;

    PVTable pvTable;

    SearchStopper *searchStopper;

    RepetitionTable repetitionTable;

    uint64_t nodesSearched;

    Eval negamax(Position &pos, Eval alpha, Eval beta, int ply, int depth);

    Eval quiescenceSearch(Position &pos, Eval alpha, Eval beta, int ply);

   public:
    Searcher(SearchStopper *searchStopper);

    void setStopper(SearchStopper *searchStopper);

    void addToRepetitionTable(uint64_t posHash);

    void clearRepetitionTable();

    void abortSearch();

    Move run(Position pos, int maxDepth);
};
