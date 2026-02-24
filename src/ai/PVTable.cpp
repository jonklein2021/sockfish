#include "PVTable.h"

#include "src/core/Notation.h"

#include <cassert>
#include <sstream>

std::string PVTable::getBestLine() {
    std::ostringstream ss;
    for (int i = 0; i < lengths[0]; i++) {
        ss << Notation::moveToUci(table[0][i]) << " ";
    }
    return ss.str();
}

Move PVTable::getBestMove() {
    return table[0][0];
}

void PVTable::update(const Move &m, int ply) {
    assert(ply < MAX_PLY - 1);

    // write move to PV table
    table[ply][ply] = m;

    // copy child PV
    for (int i = 0; i < lengths[ply + 1]; i++) {
        table[ply][ply + i + 1] = table[ply + 1][ply + i + 1];
    }

    // adjust length
    lengths[ply] = lengths[ply + 1] + 1;
}

void PVTable::clearLength(int ply) {
    lengths[ply] = 0;
}

void PVTable::clear() {
    std::fill(lengths.begin(), lengths.end(), 0);
}
