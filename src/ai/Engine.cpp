#include "Engine.h"

#include "src/core/Notation.h"

Engine::Engine(SearchStopper &searchStopper)
    : searcher(searchStopper) {}

Move Engine::getMove(Position &pos) {
    return getMove(pos, MAX_PLY);
}

Move Engine::getMove(Position &pos, int maxDepth) {
    Move move = openingBook.getMove(pos);
    if (move != Move::none()) {
        printf("info bookmove %s\n", Notation::moveToUci(move).c_str());
        return move;
    }

    return searcher.run(pos, maxDepth);
}
