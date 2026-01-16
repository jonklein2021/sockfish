#include "TranspositionTable.h"

std::optional<Eval> TranspositionTable::lookup(uint64_t posHash, int &alpha, int &beta, int depth) {
    if (table.find(posHash) != table.end()) {
        const TTEntry &e = table[posHash];
        if (e.depth >= depth) {
            if (e.flag == TTEntry::EXACT) {
                return e.eval;
            }
            if (e.flag == TTEntry::LOWERBOUND) {
                alpha = std::max(alpha, e.eval);
            }
            if (e.flag == TTEntry::UPPERBOUND) {
                beta = std::max(beta, e.eval);
            }
            if (alpha >= beta) {
                return e.eval;
            }
        }
    }
    return std::nullopt;
}

void TranspositionTable::store(uint64_t posHash, Eval eval, int alpha, int beta, int depth) {
    TTEntry entry;
    entry.eval = eval;
    entry.depth = depth;
    if (eval <= alpha) {
        entry.flag = TTEntry::UPPERBOUND;
    } else if (eval >= beta) {
        entry.flag = TTEntry::LOWERBOUND;
    } else {
        entry.flag = TTEntry::EXACT;
    }
    table[posHash] = entry;
}
