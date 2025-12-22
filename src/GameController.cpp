#pragma once

#include "GameController.h"

GameController::GameController(std::unique_ptr<Engine> engine,
                               std::shared_ptr<MoveGenerator> moveGenerator,
                               Position &startPos,
                               Color humanSide)
    : engine(std::move(engine)),
      moveGenerator(moveGenerator),
      position(startPos),
      humanSide(humanSide),
      sideToMove(WHITE) {}

constexpr bool GameController::isGameOver() const {
    return true;
}

constexpr Color GameController::getSideToMove() const {
    return sideToMove;
}

const Position &GameController::getPosition() const {
    return position;
}

const std::vector<Move> &GameController::legalMoves() const {
    return moveGenerator->generateLegal();
}

void GameController::makeHumanMove(Move move) {
    position.makeMove(move);
}

void GameController::makeAIMove() {
    Move m = engine->getMove(position, legalMoves());
    position.makeMove(m);
}
