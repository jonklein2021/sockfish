#include "Position.h"

namespace Printers {

void printBitboard(Bitboard bb);

/**
 * Converts the piece bitboards to a human-readable
 * board and prints it to stdout
 *
 * @param bb the bitboard to print
 */
void prettyPrintPosition(const Position &pos, bool flip = false);

void printPieceValues(const Position &pos);
};  // namespace Printers
