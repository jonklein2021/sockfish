#include "TranspositionTable.h"

TranspositionTable::TranspositionTable() {
    clear();
}

TTEntry TranspositionTable::lookup(uint64_t posHash, int depth) const {
    TTEntry e = table[getIndex(posHash)];
    if (e.key == posHash && e.depth <= depth) {
        return e;
    }
    return TTEntry {};
}

void TranspositionTable::store(uint64_t posHash, Eval eval, int alpha, int beta, int depth) {
    const TTFlag flag = (eval <= alpha) ? UPPERBOUND : (eval >= beta) ? LOWERBOUND : EXACT;
    table[getIndex(posHash)] = {posHash, eval, depth, flag};
}

void TranspositionTable::clear() {
    std::fill(table.begin(), table.end(), TTEntry {});
}
