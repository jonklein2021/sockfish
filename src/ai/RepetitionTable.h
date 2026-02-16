#include <array>
#include <cstdint>

// Stack-based table that stores the hashes of positions visited in game and during search
class RepetitionTable {
   private:
    static constexpr int TABLE_SIZE = 512;

    std::array<uint64_t, TABLE_SIZE> table;
    int index = 0;

   public:
    void push(uint64_t posHash);

    void pop();

    bool contains(uint64_t posHash) const;

    // consider clearing after pawn moves, captures, and castling
    void clear();
};
