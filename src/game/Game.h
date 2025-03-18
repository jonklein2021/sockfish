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
    bool playerIsWhite;
    bool playersTurn;
public:
    Game() : Game(defaultFEN) {}
    Game(const std::string &fen) : state(GameState(fen)), legalMoves(state.generateMoves()) {
        // generate seed for random features (move hints, who goes first)
        std::srand(std::time(nullptr));

        // randomly decide who goes first
        playerIsWhite = (std::rand() % 2) & 1;

        // if the player is white, it is their turn
        playersTurn = playerIsWhite;
    }

    /**
     * Runs the game loop
     */
    virtual void run() = 0;
};