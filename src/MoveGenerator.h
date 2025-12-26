#pragma once

#include "Move.h"
#include "Position.h"

#include <memory>
#include <vector>

class MoveGenerator {
   private:
    std::shared_ptr<Position> pos;

    Bitboard computeAllSidesAttacks();
    Bitboard computeAllSidesAttacks(Color color);
    Bitboard computePieceAttacks(Piece piece);
    constexpr Bitboard computePawnAttacks(Square sq, Color side);
    constexpr Bitboard computeKnightAttacks(Square sq);
    constexpr Bitboard computeBishopAttacks(Square sq);
    constexpr Bitboard computeRookAttacks(Square sq);
    constexpr Bitboard computeQueenAttacks(Square sq);
    constexpr Bitboard computeKingAttacks(Square sq);

    constexpr bool underAttack(Square sq, Color side);
    constexpr bool underAttack(Square sq);
    constexpr bool insufficientMaterial();
    constexpr bool isTerminal();
    constexpr bool isCheck();
    constexpr bool isMoveLegal(Position &copy, Square kingSq, Move &move, Color side);

   public:
    MoveGenerator(std::shared_ptr<Position> pos);

    const std::vector<Move> &generateLegal();
    const std::vector<Move> &generatePseudolegal();
};
