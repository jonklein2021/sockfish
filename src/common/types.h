#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

/**
 * Represents each the type of piece;
 * 
 * Useful for simplifying moves
 */
enum PieceType {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK, None
};

extern std::unordered_map <char, PieceType> fenPieceMap;

// N.B: This struct is only used to make rendering easier; the search algorithm will use bitboard representation
struct Piece {
    PieceType type;
    sf::Vector2<int> position; // {x, y} where 0 <= x, y < 8
    sf::Sprite sprite;
};