#include "TranspositionTable.h"

std::optional<TTEntry> TranspositionTable::lookup(uint64_t posHash, int depth) {
    if (table.find(posHash) != table.end() && table[posHash].depth <= depth) {
        return table[posHash];
    }
    return std::nullopt;
}

void TranspositionTable::store(uint64_t posHash, Eval eval, int alpha, int beta, int depth) {
    TTEntry entry{};
    entry.eval = eval;
    entry.depth = depth;
    if (eval <= alpha) {
        entry.flag = UPPERBOUND;
    } else if (eval >= beta) {
        entry.flag = LOWERBOUND;
    } else {
        entry.flag = EXACT;
    }
    table[posHash] = entry;
}
