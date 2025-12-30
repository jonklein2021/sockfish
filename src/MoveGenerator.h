#pragma once

#include "Move.h"
#include "Position.h"
#include "PositionUtil.h"

#include <memory>
#include <vector>

namespace MoveGenerator {

bool isMoveLegal(std::shared_ptr<Position> pos, Move &move);

void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq);

template<typename MoveComputer>
void appendMovesFromPiece(std::shared_ptr<Position> pos,
                          std::vector<Move> &moveList,
                          PieceType pt,
                          MoveComputer moveComputer);

std::vector<Move> generateLegal(std::shared_ptr<Position> pos);
std::vector<Move> generatePseudolegal(std::shared_ptr<Position> pos);

};  // namespace MoveGenerator
