#pragma once

#include "GameState.h"


/**
 * Abstract base class for a game that
 * classes in Cli.h and Gui.h extend
 */
class Game {
protected:
    GameState state;
    std::vector<Move> legalMoves;
public:
    /**
     * Runs the game loop
     */
    virtual void run() = 0;
};