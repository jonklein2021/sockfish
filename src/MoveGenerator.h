#pragma once

#include "Move.h"
#include "Position.h"

#include <memory>
#include <vector>

class MoveGenerator {
   private:
    std::shared_ptr<Position> pos;

    uint64_t computeAllSidesAttacks();
    uint64_t computeAllSidesAttacks(Color color);
    uint64_t computePieceAttacks(Piece piece);
    uint64_t computePawnAttacks(const uint64_t squareBit, const bool white);
    uint64_t computeKnightAttacks(const uint64_t squareBit);
    uint64_t computeBishopAttacks(const uint64_t squareBit);
    uint64_t computeRookAttacks(const uint64_t squareBit);
    uint64_t computeQueenAttacks(const uint64_t squareBit);
    uint64_t computeKingAttacks(const uint64_t squareBit);
    bool underAttack(const uint64_t squareBit, const bool white);
    bool underAttack(const uint64_t squareBit);
    bool insufficientMaterial();
    bool isTerminal();
    bool isCheck();
    bool isMoveLegal(Position &copy, const uint64_t kingBit, const Move &move, bool white);

   public:
    MoveGenerator(std::shared_ptr<Position> pos);

    const std::vector<Move> &generateLegal();
    const std::vector<Move> &generatePseudolegal();
};
