#pragma once

#include "src/core/Move.h"
#include "src/core/Position.h"

#include <string>

namespace Notation {
bool validateCoords(const std::string &input);

std::string moveToCoords(const Move &move);

Move coordsToMove(const std::string &coords);

Move uciToMove(const std::string &str);

std::string moveToUci(const Move &move);

std::string moveToSAN(const Move &move, Position &pos);

}  // namespace Notation
