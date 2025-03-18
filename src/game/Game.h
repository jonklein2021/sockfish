#pragma once

#include "GameState.h"


/**
 * Base class for a chess game that
 * classes in Cli.h and Gui.h extend
 */
class Game {
protected:
    GameState state;
    std::vector<Move> legalMoves;
public:
    Game() : Game(defaultFEN) {}
    Game(const std::string &fen) : state(GameState(fen)), legalMoves(state.generateMoves()) {}

    /**
     * Runs the game loop
     */
    virtual void run() = 0;
};