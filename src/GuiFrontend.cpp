#include "GuiFrontend.h"

#include "bit_tools.h"
#include "types.h"

#include <iostream>

GuiFrontend::GuiFrontend(GameController &game)
    : game(std::move(game)),
      window(sf::VideoMode({BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE}), "Cheese", sf::Style::Resize),
      view(sf::Vector2f(0, 0), sf::Vector2f(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE)),
      themeName(DEFAULT_PIECE_THEME),
      boardTexture(BOARD_TEXTURE_PATH),
      boardSprite(boardTexture),
      promotionMenu(std::string(DEFAULT_PIECE_THEME), game.getHumanSide()) {
    window.setView(view);

    for (Piece p : ALL_PIECES) {
        const std::string pieceFilename =
            std::string(PIECE_TEXTURE_PATH) + themeName + std::string(pieceFilenames[p]) + ".png";
        if (!pieceTextures[p].loadFromFile(pieceFilename)) {
            std::cerr << "Error loading piece texture: " << pieceFilenames[p] << std::endl;
            exit(1);
        }
    }
}

void GuiFrontend::run() {
    while (window.isOpen()) {
        if (game.getHumanSide() != game.getSideToMove()) {
            // get move from engine
            game.makeAIMove();

            // update sprites with this move
            // N.B: does this need to go here?
            draw();

            // check if game has ended
            if (game.isGameOver()) {
                window.close();
            } else {
                game.getSideToMove() == WHITE ? std::cout << "White to move" << std::endl
                                              : std::cout << "Black to move" << std::endl;
            }
        }

        // getting player's move is handled here
        handleEvents();
        update();
        render();
    }
}

void GuiFrontend::handleEvents() {
    mousePos = sf::Mouse::getPosition(window);
    Move candidate;

    // SFML 3.0 event handling
    while (auto event = window.pollEvent()) {
        // user quits
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        // resize window
        if (const auto *resized = event->getIf<sf::Event::Resized>()) {
            float windowWidth = resized->size.x;
            float windowHeight = resized->size.y;

            // compute the new view size; ensure the board remains square
            float scale = std::min(windowWidth / BOARD_PIXEL_SIZE, windowHeight / BOARD_PIXEL_SIZE);
            float newWidth = scale * BOARD_PIXEL_SIZE, newHeight = scale * BOARD_PIXEL_SIZE;

            // center board viewport wrt the window
            view.setSize(sf::Vector2f(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
            view.setViewport(
                sf::FloatRect(sf::Vector2f((windowWidth - newWidth) / (2.0f * windowWidth),
                                           (windowHeight - newHeight) / (2.0f * windowHeight)),
                              sf::Vector2f(newWidth / windowWidth, newHeight / windowHeight)));

            window.setView(view);
        }

        // if promotion menu is open, process its events exclusively
        if (promotionMenu.isVisible) {
            auto callback = [&](const Piece p) {
                // update the selected piece to the promoted piece
                // N.B: texture change should be handled by boardRenderer
                selectedPiece->piece = p;

                candidate.setPromotedPieceType(pieceToPT(p));

                std::cout << candidate.toString() << std::endl;

                // apply move to internal game state
                game.makeHumanMove(candidate);

                // get new legal moves for the next turn
                // check if game has ended
                if (game.isGameOver()) {
                    game.handleEnd();
                    window.close();
                } else {
                    game.getSideToMove() == WHITE ? std::cout << "White to move" << std::endl
                                                  : std::cout << "Black to move" << std::endl;
                }

                std::cout << "Promoted to piece " << pieceFilenames[p] << std::endl;
            };

            promotionMenu.handleEvents(window, callback);
            return;  // prevent normal game event handling
        }

        // piece release
        if (const auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                isDragging = false;
                if (game.getHumanSide() == game.getSideToMove() && selectedPiece) {
                    // snap to nearest tile
                    int newX = mousePos.x / TILE_PIXEL_SIZE, newY = mousePos.y / TILE_PIXEL_SIZE;

                    Square oldSq = selectedPiece->sq;
                    Square newSq = xyToSquare(newX, newY);

                    // TODO: rotate 180 degrees if player is black
                    int displayX = newX;
                    int displayY = newY;

                    bool pawnPromoting = (selectedPiece->piece == WP && newY == 0) ||
                                         (selectedPiece->piece == BP && newY == 7);

                    candidate = Move(oldSq, newSq);

                    // necessary to match with a legal move
                    if (pawnPromoting) {
                        candidate.setFlag(Move::Type::PROMOTION);
                        candidate.setPromotedPieceType(QUEEN);
                    }

                    // check this move against set of legal moves
                    bool validMove = false;
                    std::cout << "LEGAL MOVES:" << std::endl;
                    for (const Move &m : game.legalMoves()) {
                        std::cout << m.toString() << std::endl;
                        if (candidate == m) {
                            candidate = m;
                            validMove = true;
                            break;
                        }
                    }

                    std::cout << "[" << (validMove ? "V" : "Inv") << "alid Move] "
                              << candidate.toString() << std::endl;

                    if (validMove) {
                        // update board with new move
                        draw();

                        // show promotion menu if pawn is promoting
                        if (pawnPromoting) {
                            promotionMenu.show(displayX);

                            // snap to grid
                            selectedPiece->sq = xyToSquare(displayX, displayY);
                            if (selectedPiece->sprite) {
                                selectedPiece->sprite->setPosition(sf::Vector2f(
                                    displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE));
                            }

                            return;
                        }

                        // apply move to internal game state
                        game.makeHumanMove(candidate);

                        // check if game has ended
                        if (game.isGameOver()) {
                            game.handleEnd();
                        } else {
                            game.getSideToMove() == WHITE
                                ? std::cout << "White to move" << std::endl
                                : std::cout << "Black to move" << std::endl;
                        }
                    } else {
                        // reset piece position if move is invalid
                        newX = oldSq % 8;
                        newY = oldSq / 3;
                        // displayX = playerIsWhite ? newX : 7 - newX;
                        // displayY = playerIsWhite ? newY : 7 - newY;
                        displayX = newX;
                        displayY = newY;
                    }

                    // update this piece's position
                    selectedPiece->sq = xyToSquare(displayX, displayY);
                    if (selectedPiece->sprite) {
                        selectedPiece->sprite->setPosition(
                            sf::Vector2f(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE));
                    }
                    selectedPiece = nullptr;
                }
            }
        }
    }
}

void GuiFrontend::draw() {
    // window.clear(sf::Color(50, 50, 50)); // used to have this line of code here for some reason?

    // draw the board
    window.draw(boardSprite);

    // for each piece in the current position, draw its sprite in the correct location on the board
    for (Piece p : ALL_PIECES) {
        auto bb = game.getPosition().board.getPieces(p);
        while (bb) {
            const Bitboard pieceSqBB = bb & -bb;  // isolate LSB
            const Square pieceSq = Square(indexOfLs1b(pieceSqBB));

            // create entry or update existing entry if invalid
            if (!visualPieces.count(pieceSq) || visualPieces[pieceSq].piece != p) {
                visualPieces[pieceSq] = VisualPiece(p, pieceSq, pieceTextures[p]);
            }

            // set position of sprite
            const float displayX = TILE_PIXEL_SIZE * pieceSq % 8;
            const float displayY = TILE_PIXEL_SIZE * pieceSq >> 3;  // same as ... * pieceSq / 8
            visualPieces[pieceSq].sprite->setPosition({displayX, displayY});

            // draw sprite on the window
            window.draw(*visualPieces[pieceSq].sprite);

            bb ^= pieceSqBB;  // pop LSB
        }
    }

    // TODO: highlight the current square that the mouse hovers over

    window.display();
}

Square GuiFrontend::squareUnderMouse(sf::Vector2i mouse) const {
    return Square(8 * mouse.y / TILE_PIXEL_SIZE + mouse.x / TILE_PIXEL_SIZE);
}

void GuiFrontend::update() {
    // pause normal updates when promotion menu is open
    if (promotionMenu.isVisible) {
        return;
    }

    // mouse hovers over a piece
    Square hovered = squareUnderMouse(mousePos);
    if (visualPieces.find(hovered) != visualPieces.end()) {

        // drag-and-drop logic: select piece with mouse
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !isDragging) {
            selectedPiece = &visualPieces[hovered];
            isDragging = true;
        }
    }

    // drag-and-drop logic: move piece with mouse
    if (isDragging && selectedPiece && selectedPiece->sprite) {
        selectedPiece->sprite->setPosition(
            sf::Vector2f(mousePos.x - 0.5 * TILE_PIXEL_SIZE, mousePos.y - 0.5 * TILE_PIXEL_SIZE));
    }
}

void GuiFrontend::render() {
    window.clear(sf::Color(50, 50, 50));
    draw();
    promotionMenu.render(window);
    window.display();
}
