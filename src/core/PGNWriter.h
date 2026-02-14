#pragma once
#include "src/core/Position.h"

#include <fstream>

class PGNWriter {
   private:
    std::ofstream outFile;
    int plyCount;

    void initPGN(const std::string &whitePlayer, const std::string &blackPlayer);

   public:
    PGNWriter();

    PGNWriter(const std::string &whitePlayer, const std::string &blackPlayer);

    void writeMove(Position &pos, Move &move);

    void writeRawString(const std::string &str);

    void closeFile();
};
