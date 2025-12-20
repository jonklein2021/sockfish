#include "Move.h"
#include "Position.h"

#include <vector>

class MoveGenerator {
   private:
    Position &position;

    uint64_t computeAllSidesAttacks() const;
    uint64_t computeAllSidesAttacks(Color color) const;
    uint64_t computePieceAttacks(Piece piece) const;
    uint64_t computePawnAttacks(const uint64_t squareBit, const bool white);
    uint64_t computeKnightAttacks(const uint64_t squareBit);
    uint64_t computeBishopAttacks(const uint64_t squareBit);
    uint64_t computeRookAttacks(const uint64_t squareBit);
    uint64_t computeQueenAttacks(const uint64_t squareBit);
    uint64_t computeKingAttacks(const uint64_t squareBit);
    Piece getCapturedPiece(const uint64_t toBit, const std::vector<Piece> &oppPieces);
    bool underAttack(const uint64_t squareBit, const bool white);
    bool underAttack(const uint64_t squareBit);
    bool insufficientMaterial();
    bool isTerminal();
    bool isCheck();
    bool isMoveLegal(Position &copy, const uint64_t kingBit, const Move &move, bool white);

   public:
    MoveGenerator(Position &pos);
    Move *generateLegal();
    Move *generatePseudolegal();
};
