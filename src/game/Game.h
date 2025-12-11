#pragma once

#include <ctime>
#include <memory>

#include "Engine.h"
#include "GameState.h"
#include "constants.h"

/**
 * Base class for a chess game that
 * classes in Cli.h and Gui.h extend.
 */
class Game {
  protected:
    std::unique_ptr<Engine> cpu;
    std::unique_ptr<GameState> state;
    std::string fen = defaultFEN;
    std::vector<Move> legalMoves;
    bool playerIsWhite;
    bool playersTurn;

  public:
    Game() : Game(std::make_unique<Engine>(Engine(4)), defaultFEN, true) {};
    Game(std::unique_ptr<Engine> _cpu, std::string _fen, bool _playerIsWhite)
        : cpu(std::move(_cpu)),
          state(std::make_unique<GameState>(GameState(_fen))), fen(_fen),
          playerIsWhite(_playerIsWhite), playersTurn(_playerIsWhite) {}

    /**
     * Runs the game loop
     */
    virtual void run() = 0;
};
