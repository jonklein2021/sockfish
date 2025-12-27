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

    constexpr bool underAttack(Square sq, Color side);
    constexpr bool underAttack(Square sq);
    constexpr bool insufficientMaterial();
    constexpr bool isTerminal();
    constexpr bool isCheck();
    constexpr bool isMoveLegal(Position &copy, Square kingSq, Move &move, Color side);

    void appendMovesFromBitboard(std::vector<Move> &moveList, Bitboard moves, Square srcSq);

    template<typename MoveComputer>
    void appendMovesFromPiece(std::vector<Move> &moveList, PieceType pt, MoveComputer moveComputer);

   public:
    MoveGenerator(std::shared_ptr<Position> pos);

    std::vector<Move> generateLegal();
    std::vector<Move> generatePseudolegal();
};
