#pragma once

#include "src/core/types.h"

#include <SFML/Graphics.hpp>
#include <vector>

class PromotionMenu {
   private:
    sf::RectangleShape menu;  // Menu background
    std::vector<sf::Texture> textures;
    std::vector<sf::Sprite> pieces;
    std::vector<sf::CircleShape> circles;
    Piece selectedPiece;
    std::array<Piece, 4> pieceOptions;

   public:
    bool isVisible;

    /**
     * Constructor for the PromotionMenu class; recall that this
     * menu only shows for human players, not the engine
     *
     * @param white true if the promotion menu is for white pieces
     */
    PromotionMenu(const std::string &theme, Color side);

    void render(sf::RenderWindow &window);

    void handleEvent(const std::optional<sf::Event> &event, sf::RenderWindow &window);

    void update(sf::Cursor &arrowCursor, sf::Cursor &handCursor, sf::RenderWindow &window);

    Piece getPromotionPiece();

    // Show the promotion menu
    void show(int col);
};
