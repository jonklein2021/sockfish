#include "Position.h"
#include "types.h"

#include <functional>

namespace MoveComputers {

using MoveComputerFunc = std::function<Bitboard(const Position &, Square)>;

Bitboard computePawnAttacks(const Position &pos, Square sq);
Bitboard computeKnightMoves(const Position &pos, Square sq);
Bitboard computeBishopMoves(const Position &pos, Square sq);
Bitboard computeRookMoves(const Position &pos, Square sq);
Bitboard computeQueenMoves(const Position &pos, Square sq);
Bitboard computeKingMoves(const Position &pos, Square sq);

static std::array<MoveComputerFunc, 6> moveAttackComputers = {
    MoveComputers::computePawnAttacks, MoveComputers::computeKnightMoves,
    MoveComputers::computeBishopMoves, MoveComputers::computeRookMoves,
    MoveComputers::computeQueenMoves,  MoveComputers::computeKingMoves};
}  // namespace MoveComputers
