#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "constants.h"
#include "piece.h"

std::string pieceTheme = "horsey/";

// FEN to Piece vector conversion
std::vector<Piece> fenToPieces(const std::string& fen, std::unordered_map<std::string, sf::Texture>& textures) {
    std::vector<Piece> pieces;
    int x = 0, y = 0;
    for (const char& c : fen) {
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square; skip x squares
            x += c - '0';
        } else { // piece
            Piece piece;
            piece.isWhite = isupper(c);
            piece.type = (piece.isWhite ? 'w' : 'b') + std::string(1, static_cast<char>(toupper(c)));
            piece.position = {x, y};
            piece.sprite = sf::Sprite(textures[piece.type]);
            piece.sprite.setPosition(x * 64, y * 64);
            piece.isAlive = true;
            pieces.push_back(piece);
            x++;
        }
    }
    return pieces;
}

int main() {
    // get cursors
    sf::Cursor arrowCursor, handCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);
    handCursor.loadFromSystem(sf::Cursor::Hand);

    // create window
    sf::RenderWindow window(sf::VideoMode(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE), "Cheess", sf::Style::Resize);

    // load board texture
    sf::Texture boardTexture;
    if (!boardTexture.loadFromFile(BOARD_TEXTURE_PATH)) {
        std::cerr << "Error loading chessboard texture!" << std::endl;
        return -1;
    }

    // create board sprite from texture
    sf::Sprite boardSprite(boardTexture);
    
    // create a default square view for the board
    sf::View view(sf::FloatRect(0, 0, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
    window.setView(view);

    // map to each piece to its texture
    std::unordered_map<std::string, sf::Texture> pieceTextures;
    for (const std::string& name : pieceNames) {
        sf::Texture t;
        const std::string path = PIECE_TEXTURE_PATH + pieceTheme + name + ".png";
        if (!t.loadFromFile(path)) {
            std::cerr << "Error loading texture: " << path << std::endl;
            return -1;
        }
        pieceTextures[name] = std::move(t);
    }

    // generate piece structs from FEN (includes sprite)
    std::vector<Piece> pieces = fenToPieces(startingFen, pieceTextures);

    // used in loop as drag-and-drop variables
    Piece* selectedPiece = nullptr;
    bool isDragging = false;

    // main render loop typeshit
    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // detect if mouse is hovering over a piece
        bool hoveringOverPiece = false;
        for (auto& piece : pieces) {
            if (piece.sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                hoveringOverPiece = true;
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !isDragging) {
                    selectedPiece = &piece;
                    isDragging = true;
                }
            }
        }

        // update mouse cursor
        window.setMouseCursor(hoveringOverPiece ? handCursor : arrowCursor);

        // event handler loop
        while (window.pollEvent(event)) {
            // window close
            if (event.type == sf::Event::Closed)
                window.close();

            // left-click release
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                isDragging = false;
                if (selectedPiece) {
                    // snap to nearest tile
                    int newX = (mousePos.x / TILE_PIXEL_SIZE) * TILE_PIXEL_SIZE;
                    int newY = (mousePos.y / TILE_PIXEL_SIZE) * TILE_PIXEL_SIZE;
                    selectedPiece->sprite.setPosition(newX, newY);
                    selectedPiece = nullptr;
                }
            }

            // resize window
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

        // drag-and-drop logic; move piece with mouse
        if (isDragging && selectedPiece) {
            selectedPiece->sprite.setPosition(mousePos.x - TILE_PIXEL_SIZE / 2, mousePos.y - TILE_PIXEL_SIZE / 2);
        }

        // render sprites
        window.clear(sf::Color(50, 50, 50));
        window.draw(boardSprite);
        for (const auto& p : pieces) {
            window.draw(p.sprite);
        }
        window.display();
    }

    return 0;
}
