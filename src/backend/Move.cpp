#include <sstream>
#include "Move.h"
#include "constants.h"

Move::Move() : Move(sf::Vector2<int>{-1, -1}, sf::Vector2<int>{-1, -1}, None, false, None, false) {}

Move::Move(sf::Vector2<int> from, sf::Vector2<int> to, PieceType pieceMoved, bool isCapture, PieceType promotionPiece, bool isEnPassant)
: from(from), to(to), piece(pieceMoved), promotionPiece(promotionPiece), isCapture(isCapture), isEnPassant(isEnPassant) {}

bool Move::equals(const Move &other) {
    return (
        from.x == other.from.x &&
        from.y == other.from.y &&
        to.x == other.to.x &&
        to.y == other.to.y &&
        promotionPiece == other.promotionPiece
    );
}

std::string Move::to_string() const {
    std::ostringstream ss;
    
    ss << (piece == None ? "None" : pieceFilenames[piece]) << ": (" 
    << from.x << ", " << from.y << ") -> ("
    << to.x << ", " << to.y << ")" << (isCapture ? " (capture)" : "")
    << (promotionPiece == None ? "" : " (promotion: " + pieceFilenames[promotionPiece] + ")");

    return ss.str();
}

Move coordsToMove(const std::string& input) {
    sf::Vector2<int> from = {input[0] - 'a', '8' - input[1]};
    sf::Vector2<int> to = {input[3] - 'a', '8' - input[4]};
    return Move(from, to, None, false);
}

std::string moveToCoords(const Move& move) {
    std::string out = "";
    out += 'a' + move.from.x;
    out += '8' - move.from.y;
    out += ' ';
    out += 'a' + move.to.x;
    out += '8' - move.to.y;
    return out;
}

