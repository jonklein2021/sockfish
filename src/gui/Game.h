#include <list>
#include <unordered_map>

#include "GameState.h"
#include "PromotionMenu.h"

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

        // pawn promotion window
        PromotionMenu promotionMenu;

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
        
        // backend stuff
        GameState state;
        std::vector<Move> legalMoves;

        /**
         * Converts a FEN string representation of a chess position into a list of Piece objects
         * 
         * @param fen FEN string
         * @return list of Piece objects
         */
        std::list<Piece> fenToPieces(const std::string& fen);
        
        /**
         * Loads piece textures from disk and populates the pieceTextures map
         */
        void loadPieceTextures();
    
        void handleEvents();
        void update();
        void render();

    public:
        Game();
        Game(const std::string &fen);
        void run();
};
