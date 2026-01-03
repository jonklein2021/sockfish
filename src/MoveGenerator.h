#pragma once

#include "Move.h"
#include "Position.h"
#include "PositionUtil.h"

#include <memory>
#include <vector>

namespace MoveGenerator {

bool isMoveLegal(std::shared_ptr<Position> pos, Move &move);

template<Move::Type moveType>
void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq);

template<Move::Type moveType, PieceType pt, typename MoveComputer>
void appendMovesFromPiece(std::shared_ptr<Position> pos,
                          std::vector<Move> &moveList,
                          MoveComputer moveComputer);

Move createCastlingMove(bool isQueenside, Color side);

void appendCastlingMoves(std::vector<Move> &moveList, std::shared_ptr<Position> pos);

std::vector<Move> generateLegal(std::shared_ptr<Position> pos);
std::vector<Move> generatePseudolegal(std::shared_ptr<Position> pos);

}  // namespace MoveGenerator
