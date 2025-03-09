#include <SFML/Graphics.hpp>
#include <iostream>

const int BOARD_PIXEL_SIZE = 512; // board is 512x512 pixels

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
    
    // Create a default view matching the board's aspect ratio
    sf::View view(sf::FloatRect(0, 0, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
    window.setView(view);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // handle resizing but keep aspect ratio
            if (event.type == sf::Event::Resized) {
                float windowWidth = event.size.width;
                float windowHeight = event.size.height;
                
                // compute the new view size; ensure the aspect ratio remains square
                float scale = std::min(windowWidth / BOARD_PIXEL_SIZE, windowHeight / BOARD_PIXEL_SIZE);
                float newWidth = scale * BOARD_PIXEL_SIZE, newHeight = scale * BOARD_PIXEL_SIZE;

                // set new viewport centered in the window
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

        // Rendering
        window.clear(sf::Color(50, 50, 50)); // Dark gray background
        window.draw(boardSprite);
        window.display();
    }

    return 0;
}
