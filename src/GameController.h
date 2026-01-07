#pragma once

#include "Engine.h"
#include "types.h"

#include <memory>

class GameController {
   private:
    Position pos;
    std::unique_ptr<Engine> engine;
    std::vector<Move> moves;
    std::vector<Position::Metadata> hashHistory;
    Color humanSide;

   public:
    GameController(Position &startPos, std::unique_ptr<Engine> engine, Color humanSide);

    constexpr Color getHumanSide() const {
        return humanSide;
    }

    // can't be constexpr because shared_ptr method access is not constexpr
    Color getSideToMove() const {
        return pos.getSideToMove();
    }

    bool isGameOver();

    const Position &getPosition() const;
    std::vector<Move> legalMoves();

    void makeHumanMove(Move move);
    Move makeAIMove();

    void handleEnd();
};
