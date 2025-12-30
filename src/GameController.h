#pragma once

#include "Engine.h"
#include "types.h"

#include <memory>

class GameController {
   private:
    std::shared_ptr<Position> pos;
    std::unique_ptr<Engine> engine;
    std::vector<Move> moves;
    std::vector<Position::Metadata> hashHistory;
    Color humanSide;
    Color sideToMove;

   public:
    GameController(std::shared_ptr<Position> startPos,
                   std::unique_ptr<Engine> engine,
                   Color humanSide);

    constexpr Color getHumanSide() const {
        return humanSide;
    }

    constexpr Color getSideToMove() const {
        return sideToMove;
    }

    bool isGameOver();

    const Position &getPosition() const;
    std::vector<Move> legalMoves();

    void makeHumanMove(Move move);
    Move makeAIMove();

    void handleEnd();
};
