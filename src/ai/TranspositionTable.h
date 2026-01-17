#include "src/core/types.h"

#include <optional>
#include <unordered_map>

enum TTFlag {
    EXACT,       // PV-Nodes
    LOWERBOUND,  // Cut-Nodes
    UPPERBOUND   // All-Nodes
};

/**
 * Transposition table entry
 *
 * Stores the evaluation of a given position at some depth,
 * as well as a flag that corresponds to the type of node
 */
struct TTEntry {
    Eval eval;
    int depth;
    TTFlag flag;
};

class TranspositionTable {
   private:
    // TODO: Use a better data structure, maybe make an LRU cache from scratch
    std::unordered_map<uint64_t, TTEntry> table;

   public:
    // Looks up a position hash at some depth and returns its TTEntry if found AND that entry was
    // recorded at the same or deeper depth, std::nullopt otherwise.
    std::optional<TTEntry> lookup(uint64_t posHash, int depth);

    void store(uint64_t posHash, Eval eval, int alpha, int beta, int depth);
};
