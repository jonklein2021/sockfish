#pragma once

#include "types.h"

#include <string>

// heavily inspired from Stockfish:
// https://github.com/official-stockfish/Stockfish/blob/c109a88ebe93ab7652c7cb4694cfc405568e5e50/src/types.h#L432
class Move {
   private:
    /**
     * data[0:5]: from Square (6 bits)
     * data[6:10]: to Square (6 bits)
     * data[11:13]: promotedPieceType (2 bits)
     *  0b00 -> Knight
     *  0b01 -> Bishop
     *  0b10 -> Rook
     *  0b11 -> Queen
     * data[14:15]: special flags (2 bits)
     *  0b00 -> normal,
     *  0b01 -> promotion,
     *  0b10 -> en passant,
     *  0b11 -> castles
     */
    uint16_t data;

   public:
    enum Type { NORMAL, PROMOTION = 1 << 14, EN_PASSANT = 2 << 14, CASTLING = 3 << 14 };

    Move();

    constexpr explicit Move(uint16_t _data);

    constexpr Move(Square from, Square to);

    constexpr Move(Square from, Square to, Type moveType, PieceType promotedPieceType = KNIGHT);

    constexpr uint16_t raw() const;

    constexpr Square fromSquare() const;

    constexpr Square toSquare() const;

    constexpr Piece promotedPieceType() const;

    constexpr bool isPromotion() const;

    constexpr bool isEnPassant() const;

    // Note: When true, the type of castle can be determined by which rook moves, which is
    // represented in the from/to squares
    constexpr bool isCastles() const;

    /**
     * Convert the move to a readable string representation,
     * mainly used for debugging
     *
     * @return string representation of the move
     */
    std::string toString() const;

    bool operator==(const Move &other) const;

    bool operator!=(const Move &other) const;
};

bool validateCoords(const std::string &input);
