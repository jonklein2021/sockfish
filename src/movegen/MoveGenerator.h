#pragma once

#include "src/core/Move.h"
#include "src/core/Position.h"
#include "src/core/PositionUtil.h"

#include <vector>

namespace MoveGenerator {

template<bool IsQueenside, Color Side>
Move inline createCastlingMove() {
    constexpr Square KING_FROM = Side == WHITE ? e1 : e8;
    constexpr Square KING_TO = Side == WHITE ? (IsQueenside ? c1 : g1) : (IsQueenside ? c8 : g8);

    return Move::create<Move::CASTLING>(KING_FROM, KING_TO);
}

template<PieceType Pt, Color Side>
void generateSlidingPieceMoves(std::vector<Move> &result, Position &pos);

void generateLegal(std::vector<Move> &result, Position &pos);

template<Color Side>
void generatePseudolegal(std::vector<Move> &result, Position &pos);

void generatePseudolegal(std::vector<Move> &result, Position &pos);

void generatePseudolegalCaptures(std::vector<Move> &result, Position &pos);

}  // namespace MoveGenerator
