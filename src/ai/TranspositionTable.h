#include "src/core/types.h"

#include <optional>

enum TTFlag : int8_t {
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
    uint64_t key = 0;  // 8 bytes
    int depth = -1;    // 4 bytes
    Eval eval;         // 4 bytes
    TTFlag flag;       // 1 byte
};  // 24 bytes total

/**
 * Simple hash table used to memoize position evaluations for fast
 * resolutions during search
 *
 * Uses a simple "always-replace" scheme to maintain size
 */
class TranspositionTable {
   private:
    // Want sizeof(table) <= 128kb
    static constexpr size_t TT_SIZE = 128 * 1024;

    // (128 * 1024) bytes / 24 bytes = 5461.33333...
    // Round down to 2^12 = 4096 for faster modulos
    std::array<TTEntry, (1 << 12)> table;

    inline uint64_t getIndex(uint64_t prehash) {
        // x mod 2^12 = x & 11
        return prehash & ((1 << 12) - 1);
    }

   public:
    TranspositionTable();

    // Looks up a position hash at some depth and returns its TTEntry if found AND that entry was
    // recorded at the same or better depth, std::nullopt otherwise.
    std::optional<TTEntry> lookup(uint64_t posHash, int depth);

    // Creates and stores a TT entry. Note that this always succeeds, so it will overwrite the
    // existing entry in the case of a collision or a duplicate.
    void store(uint64_t posHash, Eval eval, int alpha, int beta, int depth);
};
