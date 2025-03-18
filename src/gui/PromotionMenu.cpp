#include "PromotionMenu.h"
#include "constants.h"
#include <iostream>

PromotionMenu::PromotionMenu() : PromotionMenu("horsey/", true) {}

PromotionMenu::PromotionMenu(const std::string &theme, bool white) : selectedPiece(None), isVisible(false) {
    // menu will take up the size of the board
    menu.setSize(sf::Vector2f(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
    menu.setPosition(0, 0); // should update to the location of the promoted pawn
    menu.setFillColor(sf::Color(0, 0, 0, 200));

    pieceOptions = white ? promotionPiecesWhite : promotionPiecesBlack;

    // load textures for the promotion pieces
    textures.resize(4);
    for (int i = 0; i < 4; i++) {
        if (!textures[i].loadFromFile(PIECE_TEXTURE_PATH + theme + pieceFilenames[pieceOptions[i]] + ".png")) {
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
        circles[i].setPosition(0, TILE_PIXEL_SIZE * i);
    }

    // create sprites for each texture
    pieces.resize(4);
    for (int i = 0; i < 4; i++) {
        pieces[i].setTexture(textures[i]);
        pieces[i].setPosition(0, TILE_PIXEL_SIZE * i); // these will get updated when the menu is shown
    }
}

void PromotionMenu::render(sf::RenderWindow &window) {
    if (isVisible) {
        window.draw(menu); // draw the background of the menu
        for (int i = 0; i < 4; i++) {
            window.draw(circles[i]);
            window.draw(pieces[i]);
        }
    }
}

void PromotionMenu::handleEvents(sf::RenderWindow &window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        // user left clicks
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            for (int i = 0; i < 4; ++i) {
                if (pieces[i].getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                    selectedPiece = pieceOptions[i];
                    isVisible = false;
                    return;
                }
            }
        }
    }
}

void PromotionMenu::update() {
    // You can update other logic for the menu if needed
}

PieceType PromotionMenu::getPromotionPiece() {
    return selectedPiece; // Return the selected piece index
}

void PromotionMenu::show(int col) {
    for (int i = 0; i < 4; i++) {
        circles[i].setPosition(col * TILE_PIXEL_SIZE, TILE_PIXEL_SIZE * i);
        pieces[i].setPosition(col * TILE_PIXEL_SIZE, TILE_PIXEL_SIZE * i);
    }
    isVisible = true;
}

void PromotionMenu::hide() {
    isVisible = false;
}
