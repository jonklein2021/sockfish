#include "TranspositionTable.h"

TranspositionTable::TranspositionTable() {
    clear();
}

TTEntry TranspositionTable::lookup(uint64_t posHash, int ply, int depth) const {
    // this needs to be a copy
    TTEntry e = table[getIndex(posHash)];

    // only valid if keys match and depth is deep enough
    if (e.key == posHash && e.depth >= depth) {
        // adjust eval for mate scores
        if (e.eval >= MATE_BOUND) {
            e.eval += ply;
        } else if (e.eval <= -MATE_BOUND) {
            e.eval -= ply;
        }

        return e;
    }
    return TTEntry {};
}

void TranspositionTable::store(
    uint64_t posHash, Eval eval, int alpha, int beta, int ply, int depth) {
    // determine flag
    TTFlag flag = (eval <= alpha) ? UPPERBOUND : (eval >= beta) ? LOWERBOUND : EXACT;

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
