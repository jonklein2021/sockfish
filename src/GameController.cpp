#pragma once

#include "GameController.h"

GameController::GameController(std::unique_ptr<Engine> engine, Position startPos, bool humanSide);

constexpr bool GameController::isGameOver() const {
    return true;
}

constexpr Color GameController::sideToMove() const {
    return toMove;
}

const Position &GameController::position() {
    return position;
}

std::vector<Move> GameController::legalMoves() const {}

void GameController::makeHumanMove(Move move) {}

void GameController::makeAIMove(Move move) {}
