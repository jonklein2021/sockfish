#pragma once

#include "Position.h"
#include "src/core/MoveList.h"

namespace Printers {

void printBitboard(Bitboard bb);

/**
 * Converts the piece bitboards to a human-readable
 * board and prints it to stdout
 *
 * @param bb the bitboard to print
 */
void prettyPrintPosition(const Position &pos, bool flip = false, bool debug = false);

void printPieceValues(const Position &pos);

void printMoveList(const MoveList &moveList, const Position &pos);
};  // namespace Printers
