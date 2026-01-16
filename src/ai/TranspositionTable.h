#include "src/core/types.h"

#include <optional>
#include <unordered_map>

class TranspositionTable {
   private:
    /**
     * Transposition table entry
     *
     * Stores the evaluation of a given position at some depth,
     * as well as whether that evaluation is an upper/lower
     * bound or an exact evaluation
     */
    struct TTEntry {
        Eval eval;
        int depth;

        enum Flag { EXACT, LOWERBOUND, UPPERBOUND } flag;
    };

    // TODO: Use a better data structure, maybe make an LRU cache from scratch
    std::unordered_map<uint64_t, TTEntry> table;

   public:
    std::optional<Eval> lookup(uint64_t posHash, int &alpha, int &beta, int depth);

    void store(uint64_t posHash, Eval eval, int alpha, int beta, int depth);
};
