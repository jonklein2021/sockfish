#include "Engine.h"

#include "src/core/Notation.h"

#include <iostream>

Engine::Engine(SearchStopper &searchStopper)
    : searcher(searchStopper) {}

Move Engine::getMove(Position &pos) {
    return getMove(pos, MAX_PLY);
}

Move Engine::getMove(Position &pos, int maxDepth) {
    Move move = openingBook.getMove(pos);
    if (move != Move::none()) {
        std::cout << "info bookmove " << Notation::moveToUci(move) << std::endl;
        return move;
    }

    return searcher.run(pos, maxDepth);
}
