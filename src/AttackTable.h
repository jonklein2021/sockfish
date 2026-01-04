#include "bit_tools.h"
#include "types.h"

// TODO
class AttackTable {
   private:
    // maps pieces to the squares they control
    std::array<Bitboard, NO_PIECE> table;

   public:
    constexpr int numAttackers(Square sq, Color attacker) {
        constexpr std::array<std::array<Piece, 6>, 2> attackers = {WHITE_PIECES, BLACK_PIECES};
        int result = 0;
        for (Piece p : attackers[attacker]) {
            result += !!getBit(table[p], sq);
        }
        return result;
    }
};
