#pragma once

#include "Game.h"

class Cli : public Game {
   private:
    /**
     * Get a move from stdin, looping until
     * a valid legal move is entered
     */
    Move getMoveFromStdin();

   public:
    Cli();
    Cli(const Engine &cpu, const std::string &fen, bool playerIsWhite);

    /**
     * Run the game loop
     */
    void run();
};
