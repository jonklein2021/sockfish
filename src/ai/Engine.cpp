#include "Engine.h"

#include "src/core/Notation.h"

void Engine::abortSearch() {
    searcher.stop();
}

Move Engine::getMove(Position &pos) {
    return getMove(pos, MAX_PLY);
}

Move Engine::getMove(Position &pos, int maxDepth) {
    Move bookMove = openingBook.getMove(pos);
    if (bookMove != Move::none()) {
        printf("info bookmove %s\n", Notation::moveToUci(bookMove).c_str());
        return bookMove;
    }

    return searcher.run(pos, maxDepth);
}
