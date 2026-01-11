#include "Position.h"
#include "types.h"

#include <functional>

namespace MoveComputers {

using MoveComputerFunc = std::function<Bitboard(const Position &, Square)>;

Bitboard computePawnPushes(const Position &pos, Square sq);
Bitboard computePawnAttacks(const Position &pos, Square sq);
Bitboard computePawnCaptures(const Position &pos, Square sq);
Bitboard computePawnEnPassant(const Position &pos, Square sq);
Bitboard computeKnightMoves(const Position &pos, Square sq);
Bitboard computeKingMoves(const Position &pos, Square sq);

// TODO: Speed up sliding piece move gen with magic numbers
Bitboard computeBishopMoves(const Position &pos, Square sq);
Bitboard computeRookMoves(const Position &pos, Square sq);
Bitboard computeQueenMoves(const Position &pos, Square sq);

static std::array<MoveComputerFunc, 6> moveAttackComputers = {
    MoveComputers::computePawnAttacks, MoveComputers::computeKnightMoves,
    MoveComputers::computeBishopMoves, MoveComputers::computeRookMoves,
    MoveComputers::computeQueenMoves,  MoveComputers::computeKingMoves};
}  // namespace MoveComputers
