#include "PGNWriter.h"

#include "src/core/Notation.h"

#include <chrono>
#include <iomanip>
#include <iostream>

PGNWriter::PGNWriter() {
    PGNWriter("Unknown", "Unknown");
}

PGNWriter::PGNWriter(const std::string &whitePlayer, const std::string &blackPlayer)
    : outFile(DEFAULT_OUT_FILE.data(), std::ios::out), plyCount(0) {
    initPGN(whitePlayer, blackPlayer);
}

void PGNWriter::initPGN(const std::string &whitePlayer, const std::string &blackPlayer) {
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open PGN file\n";
        return;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&t);

    outFile << "[Event \"Local Game\"]\n";
    outFile << "[Site \"Local\"]\n";
    outFile << "[Date \"" << std::put_time(tm, "%Y.%m.%d") << "\"]\n";
    outFile << "[Round \"1\"]\n";
    outFile << "[White \"" << whitePlayer << "\"]\n";
    outFile << "[Black \"" << blackPlayer << "\"]\n";
    outFile << "[Result \"*\"]\n\n";
}

void PGNWriter::writeMove(Position &pos, Move &move) {
    const std::string san = Notation::moveToSAN(move, pos);

    if (plyCount % 2 == 0) {
        outFile << (plyCount / 2 + 1) << ". ";
    }

    outFile << san << " ";
    plyCount++;
}

void PGNWriter::writeRawString(const std::string &str) {
    outFile << str;
}

void PGNWriter::closeFile() {
    outFile.close();
}
