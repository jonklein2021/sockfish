#include "BoardRenderer.h"

#include <iostream>

BoardRenderer::BoardRenderer(const std::string &theme)
    : boardSprite(boardTexture), themeName(theme) {
    if (!boardTexture.loadFromFile(BOARD_TEXTURE_PATH)) {
        std::cerr << "Error loading chessboard texture" << std::endl;
        exit(1);
    }

    for (Piece p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
        std::string pieceFilename =
            std::string(PIECE_TEXTURE_PATH) + themeName + std::string(pieceFilenames[p]) + ".png";
        if (!pieceTextures[p].loadFromFile(pieceFilename)) {
            std::cerr << "Error loading piece texture: " << pieceFilenames[p] << std::endl;
            exit(1);
        }
    }
}

void BoardRenderer::draw(sf::RenderWindow &window, const Position &pos) {}

Square BoardRenderer::squareUnderMouse(sf::Vector2i mouse) const {}
