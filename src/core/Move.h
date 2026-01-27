#pragma once

#include "src/core/types.h"

#include <string>

// heavily inspired from Stockfish:
// https://github.com/official-stockfish/Stockfish/blob/c109a88ebe93ab7652c7cb4694cfc405568e5e50/src/types.h#L432
class Move {
   public:
    enum Type { NORMAL, PROMOTION = 1 << 14, EN_PASSANT = 2 << 14, CASTLING = 3 << 14 };

   private:
    /**
     * data[0:5]: from Square (6 bits)
     * data[6:11]: to Square (6 bits)
     * data[12:13]: promotedPieceType (2 bits)
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
    constexpr Move()
        : data(0u) {}

    constexpr Move(uint16_t _data)
        : data(_data) {}

    constexpr Move(Square from, Square to)
        : data(from | (to << 6)) {}

    template<Type moveType>
    inline static constexpr Move create(Square from, Square to, PieceType promotedPT = KNIGHT) {
        return Move(from | (to << 6) | moveType | ((promotedPT - KNIGHT) << 12));
    }

    static constexpr Move none() {
        return Move();
    }

    static Move fromCoordinateString(const std::string &coords);

    constexpr void setFlag(Type t) {
        data |= t;
    }

    constexpr void setPromotedPieceType(PieceType promotedPieceType) {
        data |= ((promotedPieceType - KNIGHT) << 12);  // set piece
        data |= PROMOTION;                             // set flag
    }

    constexpr uint16_t raw() const {
        return data;
    }

    constexpr Square getFromSquare() const {
        return Square(data & 0x3F);
    }

    constexpr Square getToSquare() const {
        return Square((data >> 6) & 0x3F);
    }

    constexpr PieceType getPromotedPieceType() const {
        return PieceType(((data >> 12) & 0x3) + KNIGHT);
    }

    constexpr bool isPromotion() const {
        return (data & (0x3 << 14)) == PROMOTION;
    }

    constexpr bool isEnPassant() const {
        return (data & (0x3 << 14)) == EN_PASSANT;
    }

    constexpr bool isCastles() const {
        return (data & (0x3 << 14)) == CASTLING;
    }

    constexpr bool isKCastles() const {
        return isCastles() && (getToSquare() == g1 || getToSquare() == g8);
    }

    constexpr bool isQCastles() const {
        return isCastles() && (getToSquare() == c1 || getToSquare() == c8);
    }

    /**
     * Checks equality between two moves by comparing
     * their source and destination squares only
     */
    constexpr bool softEquals(const Move &other) const {
        return (data & 0xFFF) == (other.raw() & 0xFFF);
    }

    constexpr bool operator==(const Move &other) const {
        return data == other.raw();
    }

    constexpr bool operator!=(const Move &other) const {
        return data != other.raw();
    }
};
