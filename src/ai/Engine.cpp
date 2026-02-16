#include "Engine.h"

#include "src/core/Notation.h"

#include <iostream>

Engine::Engine(SearchStopper *searchStopper)
    : searcher(searchStopper) {}

void Engine::setSearchStopper(SearchStopper *searchStopper) {
    searcher.setStopper(searchStopper);
}

void Engine::addToHashHistory(uint64_t posHash) {
    searcher.addToRepetitionTable(posHash);
}

void Engine::abortSearch() {
    searcher.abortSearch();
}

Move Engine::getMove(Position &pos) {
    return getMove(pos, MAX_PLY);
}

Move Engine::getMove(Position &pos, int depth) {
    Move move = openingBook.getMove(pos);
    if (move != Move::none()) {
        std::cout << "info bookmove " << Notation::moveToUci(move) << std::endl;
        return move;
    }

    return searcher.run(pos, depth);
}
