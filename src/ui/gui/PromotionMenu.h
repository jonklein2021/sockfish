#pragma once

#include "src/core/types.h"

#include <SFML/Graphics.hpp>
#include <functional>
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

    // Public Methods
    void render(sf::RenderWindow &window);
    void handleEvents(sf::RenderWindow &window, const std::function<void(Piece)> &callback);

    // Show the promotion menu
    void show(int col);
};
