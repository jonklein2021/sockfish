#pragma once

#include "types.h"

#include <string>

// heavily inspired from Stockfish:
// https://github.com/official-stockfish/Stockfish/blob/c109a88ebe93ab7652c7cb4694cfc405568e5e50/src/types.h#L432
class Move {
   public:
    enum Type { NORMAL, PROMOTION = 1 << 14, EN_PASSANT = 2 << 14, CASTLING = 3 << 14 };

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
    Move();

    constexpr Move(uint16_t _data)
        : data(_data) {}

    constexpr Move(Square from, Square to)
        : data(from | (to << 6)) {}

    constexpr Move(Square from, Square to, Type moveType, PieceType promotedPieceType)
        : data(from | (to << 6) | moveType | ((promotedPieceType - KNIGHT) << 11)) {}

    static Move fromCoordinateString(const std::string &coords);

    void setFlag(Type t);

    void setPromotedPiece(PieceType promotedPieceType);

    constexpr uint16_t raw() const {
        return data;
    }

    constexpr Square fromSquare() const {
        return Square(data & 0x3F);
    }

    constexpr Square toSquare() const {
        return Square((data >> 6) & 0x3F);
    }

    constexpr Piece promotedPieceType() const {
        return Piece(((data >> 11) & 0x3) + KNIGHT);
    }

    constexpr bool isPromotion() const {
        return (data & (3 << 14)) == PROMOTION;
    }

    constexpr bool isEnPassant() const {
        return (data & (3 << 14)) == EN_PASSANT;
    }

    // Note: When true, the type of castle can be determined by which rook moves, which is
    // represented in the from/to squares
    constexpr bool isCastles() const {
        return (data & (3 << 14)) == CASTLING;
    }

    std::string toCoordinateString() const;

    /**
     * @return readable representation of the move
     * for debugging
     */
    std::string toString() const;

    bool operator==(const Move &other) const;

    bool operator!=(const Move &other) const;
};

bool validateCoords(const std::string &input);
