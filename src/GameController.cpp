#pragma once

#include "GameController.h"

#include <iostream>

GameController::GameController(std::shared_ptr<Position> startPos,
                               std::shared_ptr<MoveGenerator> moveGenerator,
                               std::unique_ptr<Engine> engine,
                               Color humanSide)
    : pos(startPos),
      moveGenerator(moveGenerator),
      engine(std::move(engine)),
      humanSide(humanSide),
      sideToMove(WHITE) {}

// should GameController own this logic?
bool GameController::isGameOver() {
    return legalMoves().empty();
}

const Position &GameController::getPosition() const {
    return *pos;
}

std::vector<Move> GameController::legalMoves() {
    return moveGenerator->generateLegal();
}

void GameController::makeHumanMove(Move move) {
    pos->makeMove(move);
}

// Returns the move made by the engine
Move GameController::makeAIMove() {
    Move best = engine->getMove(*pos, legalMoves());
    pos->makeMove(best);
    return best;
}

// TODO
void GameController::handleEnd() {
    // print a user-friendly message depending on how the game ended
    std::cout << "Game over!\n";
}
