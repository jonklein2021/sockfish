#pragma once

#include <ctime>

#include "Engine.h"
#include "GameState.h"

/**
 * Base class for a chess game that
 * classes in Cli.h and Gui.h extend
 */
class Game {
protected:
    Engine cpu;
    GameState state;
    std::vector<Move> legalMoves;
    bool playerIsWhite;
    bool playersTurn;

public:
    Game()
        : cpu(Engine(8)), state(GameState(defaultFEN)),
            legalMoves(state.generateMoves()) {
        // generate seed for random features (move hints, who goes first)
        std::srand(std::time(nullptr));

        // randomly decide who goes first
        playerIsWhite = (std::rand() % 2) & 1;

        // push hash of initial state to history
        state.md.history.push_back(state.hash());

        // if the player's color matches the state's whiteToMove, it is their turn
        playersTurn = (state.whiteToMove == playerIsWhite);
    }

    Game(const std::string &fen, int depth)
        : cpu(Engine(depth)), state(GameState(fen)),
            legalMoves(state.generateMoves()) {
        // generate seed for random features (move hints, who goes first)
        std::srand(std::time(nullptr));

        // push hash of initial state to history
        state.md.history.push_back(state.hash());

        // randomly decide who goes first
        playerIsWhite = (std::rand() % 2) & 1;

        // if the player's color matches the state's whiteToMove, it is their turn
        playersTurn = (state.whiteToMove == playerIsWhite);\
    }

    Game(const std::string &fen, int depth, bool playerIsWhite)
        : cpu(Engine(depth)), state(GameState(fen)),
            legalMoves(state.generateMoves()), playerIsWhite(playerIsWhite),
            playersTurn(state.whiteToMove == playerIsWhite) {

        // push hash of initial state to history
        state.md.history.push_back(state.hash());

        // generate seed for random features (move hints, who goes first)
        std::srand(std::time(nullptr));
    }

    /**
     * Runs the game loop
     */
    virtual void run() = 0;
};