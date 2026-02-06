#pragma once

#include "src/ai/Engine.h"
#include "src/core/types.h"

#include <fstream>
#include <memory>

class GameController {
   private:
    Position pos;
    std::unique_ptr<Engine> engine;
    std::vector<Move> legalMoves;
    std::ofstream outFile;

    // used in PGN file
    int plyCount;

    Color humanSide;

    void initPGN();

    void appendToPGN(const std::string &sanString);

   public:
    GameController(Position &startPos, std::unique_ptr<Engine> engine, Color humanSide);

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

    std::vector<Move> getLegalMoves() const;

    void makeHumanMove(Move move);

    void makeAIMove();

    void handleEnd();
};
