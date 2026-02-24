#include "src/core/Move.h"
#include "src/core/types.h"

#include <string>

class PVTable {
   private:
    std::array<std::array<Move, MAX_PLY>, MAX_PLY> table;
    std::array<int, MAX_PLY> lengths;

   public:
    std::string getBestLine();

    Move getBestMove();

    void update(const Move &m, int ply);

    void clearLength(int ply);

    void clear();
};
