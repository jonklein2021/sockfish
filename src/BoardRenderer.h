#include "Position.h"
#include "types.h"

#include <SFML/Graphics.hpp>

class BoardRenderer {
   public:
    BoardRenderer(const std::string &theme);

    void draw(sf::RenderWindow &window, const Position &pos);

    Square squareUnderMouse(sf::Vector2i mouse) const;

   private:
    sf::Texture boardTexture;
    sf::Sprite boardSprite;

    std::array<sf::Texture, 12> pieceTextures;

    std::string themeName;
};
