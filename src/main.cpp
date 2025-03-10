#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

const int BOARD_PIXEL_SIZE = 512; // board is 512x512 pixels
const char* PIECES_TEXTURE_DIR = "assets/pieces/alpha/";

struct Piece {
    std::string type;
    sf::Vector2<int> position;
    sf::Sprite sprite;
    bool isWhite;
    bool isAlive;
    Piece() : type(""), position({0, 0}), isWhite(false), isAlive(false) {}
    Piece(const std::string& type, const sf::Sprite& sprite, const sf::Vector2<int>& position, bool isWhite, bool isAlive)
        : type(type), sprite(sprite), position(position), isWhite(isWhite), isAlive(isAlive) {}
};

// FEN to Piece vector conversion
std::vector<Piece> fenToPieces(const std::string& fen, std::unordered_map<std::string, sf::Texture>& textures) {
    std::vector<Piece> pieces;
    int x = 0, y = 0;
    for (const char& c : fen) {
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square
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
    // create window
    sf::RenderWindow window(sf::VideoMode(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE), "Cheess", sf::Style::Resize);

    // load board texture
    sf::Texture boardTexture;
    if (!boardTexture.loadFromFile("assets/board.png")) {
        std::cerr << "Error loading chessboard texture!" << std::endl;
        return -1;
    }

    // create board sprite from texture
    sf::Sprite boardSprite(boardTexture);
    
    // create a default square view for the board
    sf::View view(sf::FloatRect(0, 0, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
    window.setView(view);

    std::vector<std::string> pieceNames = {"wK", "wQ", "wR", "wB", "wN", "wP", "bK", "bQ", "bR", "bB", "bN", "bP"};

    // map to each piece to its texture
    std::unordered_map<std::string, sf::Texture> pieceTextures;
    for (const std::string& piece : pieceNames) {
        sf::Texture t;
        std::string path = PIECES_TEXTURE_DIR + piece + ".png";
        if (!t.loadFromFile(path)) {
            std::cerr << "Error loading texture: " << path << std::endl;
            return -1;
        }
        pieceTextures[piece] = std::move(t);
    }

    // generate piece structs from FEN (includes sprite)
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    std::vector<Piece> pieces = fenToPieces(fen, pieceTextures);

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
        for (const auto& p : pieces) {
            window.draw(p.sprite);
        }
        window.display();
    }

    return 0;
}
