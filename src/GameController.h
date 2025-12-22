#pragma once

#include "Engine.h"
#include "MoveGenerator.h"
#include "types.h"

#include <memory>

class GameController {
   private:
    std::unique_ptr<Engine> engine;
    std::shared_ptr<MoveGenerator> moveGenerator;
    std::vector<Move> moves;
    std::vector<Position::Metadata> hashHistory;
    Position position;
    Color humanSide;
    Color sideToMove;

   public:
    GameController(std::unique_ptr<Engine> engine,
                   std::shared_ptr<MoveGenerator> moveGenerator,
                   Position &startPos,
                   Color humanSide);

    constexpr Color getHumanSide() const;
    constexpr Color getSideToMove() const;
    constexpr bool isGameOver() const;

    const Position &getPosition() const;
    const std::vector<Move> legalMoves();

    void makeHumanMove(Move move);
    void makeAIMove();
};
