#pragma once

#include "Engine.h"
#include "MoveGenerator.h"
#include "types.h"

#include <memory>

class GameController {
   private:
    std::shared_ptr<Position> pos;
    std::shared_ptr<MoveGenerator> moveGenerator;
    std::unique_ptr<Engine> engine;
    std::vector<Move> moves;
    std::vector<Position::Metadata> hashHistory;
    Color humanSide;
    Color sideToMove;

   public:
    GameController(std::shared_ptr<Position> startPos,
                   std::shared_ptr<MoveGenerator> moveGenerator,
                   std::unique_ptr<Engine> engine,
                   Color humanSide);

    constexpr Color getHumanSide() const;
    constexpr Color getSideToMove() const;
    bool isGameOver();

    const Position &getPosition() const;
    const std::vector<Move> legalMoves();

    void makeHumanMove(Move move);
    void makeAIMove();
};
