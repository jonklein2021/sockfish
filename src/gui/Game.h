#define PIECE_TEXTURE_PATH "assets/pieces/"
#define BOARD_TEXTURE_PATH "assets/board.png"

#define BOARD_PIXEL_SIZE 512 // board is 512x512 pixels
#define TILE_PIXEL_SIZE 64 // 512/8 = 64

#include <list>
#include <unordered_map>

#include "GameState.h"

#include <SFML/Graphics.hpp>

class Game {
    private:
        // visual stuff
        sf::RenderWindow window;
        sf::View view;
        sf::Cursor arrowCursor, handCursor;
        sf::Texture boardTexture;
        sf::Sprite boardSprite;
        std::list<Piece> pieces;

        sf::Vector2<int> mousePos;
        
        // used for piece theme
        std::string pieceTheme = "horsey/";
        const std::vector<std::string> pieceFilenames = {
            "wP", "wN", "wB", "wR", "wQ", "wK",
            "bP", "bN", "bB", "bR", "bQ", "bK"
        };

        // drag-and-drop variables
        Piece* selectedPiece = nullptr;
        bool isDragging = false;

        // 1-1 mapping from each piece to its texture
        std::unordered_map<PieceType, sf::Texture> pieceTextures;

        const std::string initFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
        
        // backend stuff
        GameState state;
        std::vector<Move> legalMoves;

        // FEN to Piece list conversion
        std::list<Piece> fenToPieces(const std::string& fen);
        
        // encapsulated function for piece texture loading
        void loadPieceTextures();
    
        void handleEvents();
        void update();
        void render();

    public:
        Game();
        void run();
};
