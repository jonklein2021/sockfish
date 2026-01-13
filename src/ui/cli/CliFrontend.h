#pragma once

#include "src/core/GameController.h"

class CliFrontend {
   private:
    GameController &game;

    bool validateMoveInput(const std::string &input);

    /**
     * Get a move from stdin, loops until
     * a valid legal move is entered
     */
    Move getMoveFromStdin();

   public:
    CliFrontend(GameController &game);

    /**
     * Run the game loop
     */
    void run();
};
