#include <sstream>
#include "Move.h"
#include "constants.h"

Move::Move() : Move({-1, -1}, {-1, -1}, None) {}

Move::Move(sf::Vector2<int> from, sf::Vector2<int> to, PieceType pieceMoved, PieceType capturedPiece,
           PieceType promotionPiece, bool isKCastle, bool isQCastle, bool isEnPassant)
    : from(from), to(to), piece(pieceMoved), promotionPiece(promotionPiece), capturedPiece(capturedPiece),
      isEnPassant(isEnPassant), isKCastle(isKCastle), isQCastle(isQCastle) {}

bool Move::equals(const Move &other) {
    return (
        from.x == other.from.x &&
        from.y == other.from.y &&
        to.x == other.to.x &&
        to.y == other.to.y &&
        promotionPiece == other.promotionPiece);
}
// note to self: this^ is really fucking sketch and I should change it

std::string Move::toString() const {
    std::ostringstream out;

    out << (piece == None ? "None" : pieceNames[piece]) << ": "
        << moveToCoords(*this)
        << (capturedPiece == None ? "" : " (captures " + pieceNames[capturedPiece] + ")")
        << (promotionPiece == None ? "" : " (promotes to " + pieceNames[promotionPiece] + ")")
        << (isKCastle ? " (kingside castle)" : "")
        << (isQCastle ? " (queenside castle)" : "")
        << (isEnPassant ? " (en passant)" : "");

    return out.str();
}

Move coordsToMove(const std::string &input) {
    sf::Vector2<int> from = {input[0] - 'a', '8' - input[1]};
    sf::Vector2<int> to = {input[3] - 'a', '8' - input[4]};
    return Move(from, to, None);
}

std::string moveToCoords(const Move &move) {
    std::string out = "";
    out += 'a' + move.from.x;
    out += '8' - move.from.y;
    out += ' ';
    out += 'a' + move.to.x;
    out += '8' - move.to.y;
    return out;
}

bool validateCoords(const std::string& input) {
    if (input.size() != 5) {
        return false;
    }

    return (input[0] >= 'a' && input[0] <= 'h') &&
        (input[1] >= '1' && input[1] <= '8') &&
        (input[2] == ' ') &&
        (input[3] >= 'a' && input[3] <= 'h') &&
        (input[4] >= '1' && input[4] <= '8');
}