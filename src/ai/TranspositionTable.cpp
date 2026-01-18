#include "TranspositionTable.h"

TranspositionTable::TranspositionTable() {
    std::fill(table.begin(), table.end(), TTEntry{});
}

std::optional<TTEntry> TranspositionTable::lookup(uint64_t posHash, int depth) {
    TTEntry e = table[getIndex(posHash)];
    if (e.key == posHash && e.depth <= depth) {
        return e;
    }
    return std::nullopt;
}

void TranspositionTable::store(uint64_t posHash, Eval eval, int alpha, int beta, int depth) {
    TTFlag flag;
    if (eval <= alpha) {
        // All-Node
        flag = UPPERBOUND;
    } else if (eval >= beta) {
        // Cut-Node
        flag = LOWERBOUND;
    } else {
        // PV-Node: alpha < eval < beta
        flag = EXACT;
    }

    table[getIndex(posHash)] = {posHash, eval, depth, flag};
}
