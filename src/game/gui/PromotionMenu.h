#include <functional>
#include <vector>
#include <SFML/Graphics.hpp>

#include "constants.h"

class PromotionMenu {
private:
    sf::RectangleShape menu;  // Menu background
    std::vector<sf::Texture> textures;
    std::vector<sf::Sprite> pieces;
    std::vector<sf::CircleShape> circles;
    PieceType selectedPiece;
    std::vector<PieceType> pieceOptions;
    
public:
    bool isVisible;
    PromotionMenu();
    
    /**
     * Constructor for the PromotionMenu class; recall that this
     * menu only shows for human players, not the engine
     * 
     * @param white true if the promotion menu is for white pieces
     */
    PromotionMenu(const std::string &theme, bool white);
    
    // Public Methods
    void render(sf::RenderWindow& window);
    void handleEvents(sf::RenderWindow &window, const std::function<void(PieceType)> &callback);
    
    // Show the promotion menu
    void show(int col);
};