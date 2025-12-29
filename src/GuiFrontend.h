#pragma once

#include "GameController.h"
#include "PromotionMenu.h"
#include "VisualPiece.h"

#include <SFML/Graphics.hpp>

class GuiFrontend {
   private:
    // game logic orchestrator
    GameController game;

    // SFML formalities
    sf::RenderWindow window;
    sf::View view;

    // visual theme variables
    std::string themeName;
    sf::Texture boardTexture;
    sf::Sprite boardSprite;

    // array of pieces on the screen alongside their sprites
    std::array<sf::Texture, 12> pieceTextures;
    std::unordered_map<Square, VisualPiece> visualPieces;

    // self-explanatory
    PromotionMenu promotionMenu;

    // drag-and-drop variables
    sf::Vector2i mousePos;
    VisualPiece *selectedPiece = nullptr;
    bool isDragging = false;

    /**
     * Sets up window and textures
     */
    void initializeScreen();

    /**
     * Loads piece textures from disk and populates the pieceTextures map
     */
    void loadPieceTextures();

    /**
     * Draws the current position on the screen
     */
    void draw();

    /**
     * Returns the Square the mouse is hovering over
     */
    Square squareUnderMouse(sf::Vector2i mouse) const;

    /**
     * Primary event loop; handles window resizing,
     * window closing, mouse clicks and piece dragging
     */
    void handleEvents();

    /**
     * Updates the game state
     */
    void update();

    /**
     * Renders relevant sprites to the window, including the board,
     * pieces, and promotion menu if it is open
     */
    void render();

   public:
    GuiFrontend(GameController &game);
    void run();
};
