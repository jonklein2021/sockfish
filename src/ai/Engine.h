#pragma once

#include "src/ai/PolyglotBook.h"
#include "src/ai/search/Searcher.h"
#include "src/core/Position.h"

class Engine {
   private:
    Searcher searcher;

    PolyglotBook openingBook;

   public:
    Engine(SearchStopper *searchStopper);

    void setSearchStopper(SearchStopper *searchStopper);

    void addToHashHistory(uint64_t posHash);

    void clearHistory();

    void abortSearch();

    Move getMove(Position &pos);

    /**
     * Gets the best move in a certain position
     *
     * @param pos to evaluate
     * @param depth to search to
     * @return The best move to make
     */
    Move getMove(Position &pos, int depth);

    // same as getMove but never queries openingBook
    Move getSearchedMove(Position &pos, int depth);
};
