#pragma once

#include "src/core/GameController.h"

class CliFrontend {
   private:
    GameController &game;

    bool validateMoveInput(const std::string &input);

    std::string formatMove(const Move m);

    std::string getMoveSuggestion(const MoveList &legalMoves);

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
