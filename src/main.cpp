#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

const int BOARD_PIXEL_SIZE = 512; // board is 512x512 pixels
const char* PIECES_TEXTURE_DIR = "assets/pieces/alpha/";

int main() {
    // create window
    sf::RenderWindow window(sf::VideoMode(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE), "Cheess", sf::Style::Resize);

    // load board texture
    sf::Texture boardTexture;
    if (!boardTexture.loadFromFile("assets/board.png")) {
        std::cerr << "Error loading chessboard texture!" << std::endl;
        return -1;
    }

    sf::Sprite boardSprite(boardTexture);
    
    // create a default view matching the board's aspect ratio
    sf::View view(sf::FloatRect(0, 0, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
    window.setView(view);

    // create a map to hold textures for each piece
    std::unordered_map<std::string, sf::Texture> pieceTextures;
    std::vector<std::string> pieces = {"wK", "wQ", "wR", "wB", "wN", "wP", "bK", "bQ", "bR", "bB", "bN", "bP"};

    for (const std::string& piece : pieces) {
        sf::Texture texture;
        if (!texture.loadFromFile(std::string(PIECES_TEXTURE_DIR) + piece + ".png")) {
            std::cerr << "Error loading piece texture: " << piece << std::endl;
            return -1;
        }
        pieceTextures[piece] = std::move(texture);
    }

    // create a map to hold sprites for each piece
    std::unordered_map<std::string, sf::Sprite> pieceSprites;
    for (const auto& piece : pieces) {
        sf::Sprite sprite(pieceTextures[piece]);
        pieceSprites[piece] = std::move(sprite);
    }

    // set the position of each piece sprite
    for (int i = 0; i < 8; i++) {
        pieceSprites["wP"].setPosition(64 * i, 64 * 6);
        pieceSprites["bP"].setPosition(64 * i, 64);
    }

    pieceSprites["bR"].setPosition(0, 0);
    pieceSprites["bR"].setPosition(448, 0);
    pieceSprites["wR"].setPosition(0, 448);
    pieceSprites["wR"].setPosition(448, 448);

    // main loop typeshit
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // resize handler
            if (event.type == sf::Event::Resized) {
                float windowWidth = event.size.width;
                float windowHeight = event.size.height;
                
                // compute the new view size; ensure the board remains square
                float scale = std::min(windowWidth / BOARD_PIXEL_SIZE, windowHeight / BOARD_PIXEL_SIZE);
                float newWidth = scale * BOARD_PIXEL_SIZE, newHeight = scale * BOARD_PIXEL_SIZE;

                // center board viewport wrt the window
                view.setSize(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE);
                view.setViewport({
                    (windowWidth - newWidth) / (2.0f * windowWidth),
                    (windowHeight - newHeight) / (2.0f * windowHeight),
                    newWidth / windowWidth,
                    newHeight / windowHeight
                });

                window.setView(view);
            }
        }

        // rendering
        window.clear(sf::Color(50, 50, 50));
        window.draw(boardSprite);
        for (const auto& piece : pieces) {
            window.draw(pieceSprites[piece]);
        }
        window.display();
    }

    return 0;
}
