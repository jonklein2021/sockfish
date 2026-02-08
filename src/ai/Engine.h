#pragma once

#include "src/ai/PolyglotBook.h"
#include "src/ai/search/Searcher.h"
#include "src/core/Position.h"

class Engine {
   private:
    /**
     * The maximum depth to search
     *
     * TODO: Increase this a lot after creating an interrupt mechanism
     */
    static constexpr int MAX_PLY = 64;

    Searcher searcher;

    PolyglotBook openingBook;

   public:
    Engine(SearchStopper &searchStopper);

    Move getMove(Position &pos);

    /**
     * Gets the best move in a certain position
     *
     * @param pos to evaluate
     * @param maxDepth to search to
     * @return The best move to make
     */
    Move getMove(Position &pos, int maxDepth);
};
