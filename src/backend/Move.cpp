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