#pragma once

#include "BoardRenderer.h"
#include "GameController.h"
#include "PromotionMenu.h"

#include <SFML/Graphics.hpp>

class GuiFrontend {
   private:
    // visual stuff
    sf::RenderWindow window;
    sf::View view;
    sf::Vector2<int> mousePos;

    // pieces on the board
    // std::list<Piece> pieces;

    PromotionMenu promotionMenu;
    BoardRenderer boardRenderer;

    GameController game;

    // drag-and-drop variables
    Piece *selectedPiece = nullptr;
    bool isDragging = false;

    // 1-1 mapping from each piece to its texture
    // std::unordered_map<Piece, sf::Texture> pieceTextures;

    /**
     * Sets up window and textures
     */
    void initializeScreen();

    /**
     * Loads piece textures from disk and populates the pieceTextures map
     */
    void loadPieceTextures();

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
