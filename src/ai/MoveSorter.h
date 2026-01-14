#include "src/core/Move.h"
#include "src/core/Position.h"

#include <vector>

using MoveScore = int;

class MoveSorter {
   private:
    /**
     * Assigns a move a value based on how promising
     * it looks in that position; mainly uses MVV-LVA
     *
     * @param pos in which a move should be judged
     * @param move The move to rate
     * @return A value representing how promising the move is
     */
    MoveScore rateMove(const Position &pos, const Move &move);

   public:
    constexpr MoveSorter() {}

    /**
     * Sorts list of moves in a given position according rateMove
     */
    void run(const Position &pos, std::vector<Move> &moveList);
};
