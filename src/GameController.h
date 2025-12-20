#pragma once

#include "Engine.h"
#include "types.h"

#include <memory>

/**
 * Base class for a chess game that
 * classes in Cli.h and Gui.h extend.
 */
class GameController {
   private:
    std::unique_ptr<Engine> engine;
    Position position;
    std::vector<Move> moves;
    std::vector<Position::Metadata> history;
    Color human;
    Color toMove;

   public:
    GameController(std::unique_ptr<Engine> engine, Position startPos, bool humanSide);

    constexpr bool isGameOver() const;
    constexpr Color humanSide() const;
    constexpr Color sideToMove() const;

    const Position &position() const;
    std::vector<Move> legalMoves() const;

    void makeHumanMove(Move move);
    void makeAIMove(Move move);
};
