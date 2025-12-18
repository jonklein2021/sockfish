#include "PromotionMenu.h"

#include <iostream>

PromotionMenu::PromotionMenu()
    : PromotionMenu("horsey/", true) {}

PromotionMenu::PromotionMenu(const std::string &theme, bool white)
    : selectedPiece(NO_PIECE), isVisible(false) {
    // menu will take up the size of the board
    menu.setSize(sf::Vector2f(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
    menu.setPosition(sf::Vector2f(0, 0));
    menu.setFillColor(sf::Color(0, 0, 0, 200));

    pieceOptions = white ? promotionPiecesWhite : promotionPiecesBlack;

    // load textures for the promotion pieces
    textures.resize(4);
    for (int i = 0; i < 4; i++) {
        if (!textures[i].loadFromFile(PIECE_TEXTURE_PATH + theme + pieceFilenames[pieceOptions[i]] +
                                      ".png")) {
            std::cerr << "Error loading promotion piece textures" << std::endl;
        }
    }

    // create circle shapes under each texture
    circles.resize(4);
    for (int i = 0; i < 4; i++) {
        circles[i].setRadius(TILE_PIXEL_SIZE * 0.48);
        circles[i].setFillColor(sf::Color(255, 255, 255, 127));
        circles[i].setOutlineThickness(0);
        circles[i].setOutlineColor(sf::Color::White);
        circles[i].setPosition(sf::Vector2f(0, TILE_PIXEL_SIZE * i));
    }

    // create sprites for each texture - need to initialize with textures
    pieces.reserve(4);
    for (int i = 0; i < 4; i++) {
        pieces.emplace_back(textures[i]);
        pieces[i].setPosition(sf::Vector2f(0, TILE_PIXEL_SIZE * i));
    }
}

void PromotionMenu::render(sf::RenderWindow &window) {
    if (isVisible) {
        window.draw(menu);  // draw the background of the menu
        for (int i = 0; i < 4; i++) {
            window.draw(circles[i]);
            window.draw(pieces[i]);
        }
    }
}

void PromotionMenu::handleEvents(sf::RenderWindow &window,
                                 const std::function<void(const Piece)> &callback) {
    // SFML 3.0 event handling
    while (auto event = window.pollEvent()) {
        // user left clicks
        if (const auto *mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

                std::cout << "Mouse clicked at (" << worldPos.x << ", " << worldPos.y << ")"
                          << std::endl;

                for (int i = 0; i < 4; ++i) {
                    if (pieces[i].getGlobalBounds().contains(worldPos)) {
                        selectedPiece = pieceOptions[i];
                        callback(selectedPiece);
                        isVisible = false;
                        return;
                    }
                }
            }
        }
    }
}

void PromotionMenu::show(int col) {
    for (int i = 0; i < 4; i++) {
        circles[i].setPosition(sf::Vector2f(col * TILE_PIXEL_SIZE, TILE_PIXEL_SIZE * i));
        pieces[i].setPosition(sf::Vector2f(col * TILE_PIXEL_SIZE, TILE_PIXEL_SIZE * i));
    }
    isVisible = true;
}
