#include "TranspositionTable.h"

TranspositionTable::TranspositionTable() {
    clear();
}

TTEntry TranspositionTable::lookup(uint64_t posHash, int depth) const {
    TTEntry e = table[getIndex(posHash)];
    if (e.key == posHash && e.depth >= depth) {
        return e;
    }
    return TTEntry {};
}

void TranspositionTable::store(
    uint64_t posHash, Eval eval, int alpha, int beta, int ply, int depth) {
    const TTFlag flag = (eval <= alpha) ? UPPERBOUND : (eval >= beta) ? LOWERBOUND : EXACT;

    // adjust eval for mate scores
    if (eval >= MATE_BOUND) {
        eval -= ply;
    } else if (eval <= -MATE_BOUND) {
        eval += ply;
    }

    table[getIndex(posHash)] = {posHash, depth, eval, flag};
}

void TranspositionTable::clear() {
    std::fill(table.begin(), table.end(), TTEntry {});
}
