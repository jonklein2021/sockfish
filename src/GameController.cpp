#include "GameController.h"

#include "MoveGenerator.h"
#include "src/types.h"

#include <chrono>
#include <iomanip>
#include <iostream>

GameController::GameController(Position &startPos, std::unique_ptr<Engine> engine, Color humanSide)
    : pos(startPos),
      engine(std::move(engine)),
      legalMoves(MoveGenerator::generateLegal(startPos)),
      outFile(DEFAULT_OUT_FILE.data(), std::ios::out),
      humanSide(humanSide) {
    initOutFile();
    hashHistory.push_back(startPos.getHash());
}

GameController::~GameController() {
    outFile.close();
}

void GameController::initOutFile() {
    // Check if log file is open
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open log file for writing" << std::endl;
        // exit(1);
    }

    std::chrono::system_clock::time_point now_time_point = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now_time_point);
    std::tm *local_tm = std::localtime(&now_c);

    // Format the date and time using std::put_time
    outFile << "Local date and time: " << std::put_time(local_tm, "%Y-%m-%d %H:%M:%S") << "\n";

    // output the starting position string
    outFile << pos.toFenString() << "\n";
}

bool GameController::isGameOver() {
    return PositionUtil::isTerminal(pos);
}

std::vector<Move> GameController::getLegalMoves() const {
    return legalMoves;
}

void GameController::makeHumanMove(Move move) {
    pos.makeMove(move);
    moveHistory.push_back(move);
    legalMoves = MoveGenerator::generateLegal(pos);
    outFile << move.toString() << "\n";
    hashHistory.push_back(pos.getHash());
}

void GameController::makeAIMove() {
    Move best = engine->getMove(pos, legalMoves);
    pos.makeMove(best);
    moveHistory.push_back(best);
    outFile << best.toString() << "\n";
    legalMoves = MoveGenerator::generateLegal(pos);
    hashHistory.push_back(pos.getHash());
}

// TODO: print a user-friendly message depending on how the game ended
void GameController::handleEnd() {
    std::cout << "Game over!\n";
}
