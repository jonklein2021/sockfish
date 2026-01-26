#pragma once

#include "Palette.h"
#include "PromotionMenu.h"
#include "src/core/GameController.h"

#include <SFML/Graphics.hpp>

enum MouseButton { LMB_DOWN, RMB_DOWN, LMB_UP, RMB_UP, NO_BUTTON };

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

    void snapToSquare(Square sq) {
        sprite->setPosition(sf::Vector2f(fileOf(sq) * TILE_SIZE, rankOf(sq) * TILE_SIZE));
    }
};

class GuiFrontend {
   private:
    // game logic orchestrator
    GameController &game;

    // SFML formalities
    sf::RenderWindow window;
    sf::View view;

    // visual theme variables
    sf::Texture boardTexture;
    sf::Sprite boardSprite;

    // when a piece is picked up, its square and everywhere that it can move to will be highlighted
    std::array<sf::RectangleShape, NO_SQ> allHighlightedTiles;
    std::vector<Square> currentlyHighlightedTiles;

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

    // useful state variables
    sf::Vector2f mousePos;
    MouseButton mouseButtonStatus = NO_BUTTON;
    Move candidate;

    // drag-and-drop variables
    VisualPiece *selectedPiece = nullptr;
    bool isDragging = false;

    /**
     * Sets up window and textures
     */
    void initializeScreen(const std::string &themeName);

    /**
     * Loads piece textures from disk and populates the pieceTextures map
     */
    void loadPieceTextures();

    /**
     * Draws the current position on the screen
     */
    void syncPositionToGUI();

    Move buildCandidateMove(Square from, Square to);

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
    GuiFrontend(GameController &game, const std::string &theme);
    void run();
};
