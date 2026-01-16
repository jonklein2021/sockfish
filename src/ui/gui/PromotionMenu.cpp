#include "PromotionMenu.h"

#include <SFML/Window/Mouse.hpp>
#include <iostream>

PromotionMenu::PromotionMenu(const std::string &theme, Color side)
    : selectedPiece(NO_PIECE), isVisible(false) {
    // menu will take up the size of the board
    menu.setSize(sf::Vector2f(BOARD_SIZE, BOARD_SIZE));
    menu.setPosition(sf::Vector2f(0, 0));
    menu.setFillColor(sf::Color(0, 0, 0, 200));

    pieceOptions = (side == WHITE) ? WHITE_PROMOTION_PIECES : BLACK_PROMOTION_PIECES;

    // load textures for the promotion pieces
    textures.resize(4);
    for (int i = 0; i < 4; i++) {
        const std::string filename = std::string(PIECE_TEXTURE_PATH) + theme + "/" +
                                     std::string(PIECE_FILENAMES[pieceOptions[i]]) + ".png";
        if (!textures[i].loadFromFile(filename)) {
            std::cerr << "Error loading promotion piece textures" << std::endl;
        }
    }

    // create circle shapes under each texture
    circles.resize(4);
    for (int i = 0; i < 4; i++) {
        circles[i].setRadius(TILE_SIZE * 0.48);
        circles[i].setFillColor(sf::Color(255, 255, 255, 127));
        circles[i].setOutlineThickness(0);
        circles[i].setOutlineColor(sf::Color::White);
        circles[i].setPosition(sf::Vector2f(0, TILE_SIZE * i));
    }

    // create sprites for each texture - need to initialize with textures
    pieces.reserve(4);
    for (int i = 0; i < 4; i++) {
        pieces.emplace_back(textures[i]);
        pieces[i].setPosition(sf::Vector2f(0, TILE_SIZE * i));
    }
}

void PromotionMenu::render(sf::RenderWindow &window) {
    if (!isVisible) {
        return;
    }

    // draw the menu background
    window.draw(menu);

    // draw the pieces
    for (int i = 0; i < 4; i++) {
        window.draw(circles[i]);
        window.draw(pieces[i]);
    }
}

void PromotionMenu::handleEvent(const std::optional<sf::Event> &event, sf::RenderWindow &window) {
    if (!isVisible) {
        return;
    }

    if (const auto *mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2i mousePos(mousePressed->position.x, mousePressed->position.y);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        for (int i = 0; i < 4; i++) {
            if (pieces[i].getGlobalBounds().contains(worldPos)) {
                selectedPiece = pieceOptions[i];
                break;
            }
        }
    }
}

void PromotionMenu::update(sf::Cursor &arrowCursor,
                           sf::Cursor &handCursor,
                           sf::RenderWindow &window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    for (int i = 0; i < 4; i++) {
        if (pieces[i].getGlobalBounds().contains(mousePos)) {
            window.setMouseCursor(handCursor);
            return;
        }
    }
    window.setMouseCursor(arrowCursor);
}

Piece PromotionMenu::getPromotionPiece() {
    return selectedPiece;
}

void PromotionMenu::show(int col) {
    for (int i = 0; i < 4; i++) {
        circles[i].setPosition(sf::Vector2f(col * TILE_SIZE, TILE_SIZE * i));
        pieces[i].setPosition(sf::Vector2f(col * TILE_SIZE, TILE_SIZE * i));
    }
    isVisible = true;
}
