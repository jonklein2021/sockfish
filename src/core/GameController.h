#pragma once

#include "src/ai/Engine.h"
#include "src/core/PGNWriter.h"
#include "src/core/types.h"

class GameController {
   private:
    Position pos;
    Engine &engine;
    MoveList legalMoves;
    PGNWriter pgnWriter;

    Color humanSide;

   public:
    GameController(Position &startPos, Engine &engine, Color humanSide);

    constexpr Color getHumanSide() const {
        return humanSide;
    }

    constexpr Color getSideToMove() const {
        return pos.getSideToMove();
    }

    constexpr Position &getPosition() {
        return pos;
    }

    bool isGameOver();

    MoveList getLegalMoves() const;

    void makeManualMove(Move move);

    void makeAIMove();

    void handleEnd();
};
