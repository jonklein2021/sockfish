#pragma once

#include "Engine.h"
#include "GameState.h"
#include "types.h"

#include <ctime>

/**
 * Base class for a chess game that
 * classes in Cli.h and Gui.h extend.
 */
class Game {
   protected:
    Engine cpu;
    GameState state;
    std::string fen = defaultFEN.data();
    std::vector<Move> legalMoves;
    bool playerIsWhite;
    bool playersTurn;

   public:
    Game()
        : Game(Engine(4), defaultFEN, true) {}

    Game(Engine _cpu, const std::string &_fen, bool _playerIsWhite)
        : cpu(std::move(_cpu)),
          state(GameState(_fen)),
          fen(_fen),
          playerIsWhite(_playerIsWhite),
          playersTurn(_playerIsWhite) {}

    /**
     * Runs the game loop
     */
    virtual void run() = 0;
};
