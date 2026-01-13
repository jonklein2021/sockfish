#pragma once

#include "PromotionMenu.h"
#include "src/core/GameController.h"

#include <SFML/Graphics.hpp>

/**
 * Represents a chess piece on the board, visually
 */
struct VisualPiece {
    Piece piece;
    Square sq;
    std::unique_ptr<sf::Sprite> sprite;

    VisualPiece()
        : piece(NO_PIECE), sprite(nullptr) {}

    // Constructor with texture
    explicit VisualPiece(Piece piece, Square sq, const sf::Texture &texture)
        : piece(piece), sq(sq), sprite(std::make_unique<sf::Sprite>(texture)) {}
};

class GuiFrontend {
   private:
    // game logic orchestrator
    GameController &game;

    // SFML formalities
    sf::RenderWindow window;
    sf::View view;

    // visual theme variables
    std::string themeName;
    sf::Texture boardTexture;
    sf::Sprite boardSprite;

    // stores texture for each Piece enum
    std::array<sf::Texture, NO_PIECE> pieceTextures;

    // stores on-screen pieces and their corresponding sprites
    std::vector<VisualPiece> visualPieces;

    // maps each square to where the piece on it can move in the current position
    // used to render visual move hints
    std::vector<std::vector<Square>> legalMovesBySrcSq;

    // self-explanatory
    sf::Cursor arrowCursor, handCursor;  // make this static?
    PromotionMenu promotionMenu;

    // drag-and-drop variables
    sf::Vector2f mousePos;
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
    void syncPositionToGUI();

    Move buildCandidateMove(const VisualPiece *piece, Square dst) const;

    Move validateMove(const Move &candidate);

    /**
     * Returns the Square the mouse is hovering over
     */
    Square squareUnderMouse() const;

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
