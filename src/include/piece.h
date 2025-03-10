#include <SFML/Graphics.hpp>
#include <string>

// N.B: This struct is only used to make rendering easier; the search algorithm will use bitboard representation
struct Piece {
    std::string type; // "wK", "bQ", "wP", etc.
    sf::Vector2<int> position; // {x, y} where 0 <= x, y < 8
    sf::Sprite sprite;
    bool isWhite;
    bool isAlive;
};