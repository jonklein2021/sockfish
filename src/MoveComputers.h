#include "Position.h"
#include "types.h"

#include <memory>

namespace MoveComputers {

Bitboard computeKnightMoves(std::shared_ptr<Position> pos, Square sq);
Bitboard computeBishopMoves(std::shared_ptr<Position> pos, Square sq);
Bitboard computeRookMoves(std::shared_ptr<Position> pos, Square sq);
Bitboard computeQueenMoves(std::shared_ptr<Position> pos, Square sq);
Bitboard computeKingMoves(std::shared_ptr<Position> pos, Square sq);

}  // namespace MoveComputers
