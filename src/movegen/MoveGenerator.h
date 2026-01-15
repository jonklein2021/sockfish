#pragma once

#include "src/core/Move.h"
#include "src/core/Position.h"
#include "src/core/PositionUtil.h"

#include <vector>

namespace MoveGenerator {

bool isMoveLegal(Position &pos, Move &move);
bool isCaptureMove(Position &pos, Move &move);

template<Move::Type moveType>
void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq);

template<PieceType pt, Move::Type moveType, typename MoveComputer>
void appendMovesFromPiece(std::vector<Move> &moveList, Position &pos, MoveComputer moveComputer);

Move createCastlingMove(bool isQueenside, Color side);

void appendCastlingMoves(std::vector<Move> &moveList, Position &pos);

std::vector<Move> generateLegal(Position &pos);
std::vector<Move> generatePseudolegal(Position &pos);
std::vector<Move> generateLegalCaptures(Position &pos);

}  // namespace MoveGenerator
