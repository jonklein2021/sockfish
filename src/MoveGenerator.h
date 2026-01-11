#pragma once

#include "Move.h"
#include "Position.h"
#include "PositionUtil.h"

#include <vector>

namespace MoveGenerator {

bool isMoveLegal(Position &pos, Move &move);

template<Move::Type moveType>
void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq);

template<PieceType pt, Move::Type moveType, typename MoveComputer>
void appendMovesFromPiece(std::vector<Move> &moveList, Position &pos, MoveComputer moveComputer);

Move createCastlingMove(bool isQueenside, Color side);

void appendCastlingMoves(std::vector<Move> &moveList, Position &pos);

std::vector<Move> generateLegal(Position &pos);
std::vector<Move> generatePseudolegal(Position &pos);

}  // namespace MoveGenerator
