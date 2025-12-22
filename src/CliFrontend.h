#pragma once

#include "GameController.h"

class CliFrontend {
   private:
    GameController game;

    /**
     * Get a move from stdin, loops until
     * a valid legal move is entered
     */
    Move getMoveFromStdin();

    void printBoard();

   public:
    CliFrontend(GameController &game);

    /**
     * Run the game loop
     */
    void run();
};
