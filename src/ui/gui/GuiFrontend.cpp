#include "GuiFrontend.h"

#include "src/core/types.h"

#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <optional>
#include <vector>

GuiFrontend::GuiFrontend(GameController &game, const std::string &themeName)
    : game(game),
      window(sf::VideoMode({BOARD_SIZE, BOARD_SIZE}), "Cheese", sf::Style::Resize),
      view({BOARD_SIZE / 2.f, BOARD_SIZE / 2.f}, {BOARD_SIZE, BOARD_SIZE}),
      boardTexture(BOARD_TEXTURE_PATH),
      boardSprite(boardTexture),
      arrowCursor(sf::Cursor::Type::Arrow),
      handCursor(sf::Cursor::Type::Hand),
      promotionMenu(std::string(themeName), game.getHumanSide()) {
    initializeScreen(themeName);
    syncPositionToGUI();
}

void GuiFrontend::initializeScreen(const std::string &themeName) {
    window.setView(view);

    // load piece textures
    for (Piece p : ALL_PIECES) {
        const std::string pieceFilename = std::string(PIECE_TEXTURE_PATH) + themeName + "/" +
                                          std::string(PIECE_FILENAMES[p]) + ".png";
        if (!pieceTextures[p].loadFromFile(pieceFilename)) {
            std::cerr << "Error loading piece texture: " << PIECE_FILENAMES[p] << std::endl;
            exit(1);
        }
    }

    // load board texture
    if (!boardTexture.loadFromFile(BOARD_TEXTURE_PATH)) {
        std::cerr << "Error loading board texture" << std::endl;
        exit(1);
    }

    // set up board sprite
    boardSprite.setTexture(boardTexture);
    boardSprite.setPosition({0.0, 0.0});
    sf::Vector2u texSize = boardTexture.getSize();
    boardSprite.setScale({float(BOARD_SIZE) / texSize.x, float(BOARD_SIZE) / texSize.y});

    // set up cursor
    window.setMouseCursor(arrowCursor);
}

void GuiFrontend::run() {
    while (window.isOpen()) {

        // getting player's move is handled here
        handleEvents();
        update();
        render();
    }
}

Move GuiFrontend::buildCandidateMove(Square from, Square to) {
    Move candidate(from, to);

    // check this move against set of legal moves
    return validateMove(candidate);
}

Move GuiFrontend::validateMove(const Move &candidate) {
    for (const Move &m : game.getLegalMoves()) {
        if (candidate.softEquals(m)) {
            return m;  // return canonical legal move
        }
    }
    return Move::none();
}

void GuiFrontend::handleEvents() {
    mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // SFML 3.0 event handling
    while (const auto event = window.pollEvent()) {
        // user quits
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        // resize window
        if (const auto *resized = event->getIf<sf::Event::Resized>()) {
            float windowWidth = resized->size.x;
            float windowHeight = resized->size.y;

            // compute the new view size; ensure the board remains square
            float scale = std::min(windowWidth, windowHeight) / BOARD_SIZE;
            float newWidth = scale * BOARD_SIZE, newHeight = scale * BOARD_SIZE;

            // center board viewport wrt the window
            view.setSize(sf::Vector2f(BOARD_SIZE, BOARD_SIZE));
            view.setViewport(
                sf::FloatRect(sf::Vector2f((windowWidth - newWidth) / (2.0f * windowWidth),
                                           (windowHeight - newHeight) / (2.0f * windowHeight)),
                              sf::Vector2f(newWidth / windowWidth, newHeight / windowHeight)));

            window.setView(view);
        }

        // handle promotionMenu's events exclusively when open
        if (promotionMenu.isVisible) {
            promotionMenu.handleEvent(event, window);
            return;
        }

        // mouse events
        if (const auto *mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            mouseButtonStatus =
                mousePressed->button == sf::Mouse::Button::Left ? LMB_DOWN : RMB_DOWN;
        } else if (const auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            mouseButtonStatus = mouseReleased->button == sf::Mouse::Button::Left ? LMB_UP : RMB_UP;
        } else {
            mouseButtonStatus = NO_BUTTON;
        }
    }
}

void GuiFrontend::syncPositionToGUI() {
    // reset visualPieces to match the current position
    visualPieces.clear();
    for (Square sq : ALL_SQUARES) {
        Piece p = game.getPosition().pieceAt(sq);
        if (p != NO_PIECE) {
            // create VisualPiece with this piece's sprite
            VisualPiece vp(p, sq, pieceTextures[p]);
            vp.sprite->setPosition(sf::Vector2f(fileOf(sq) * TILE_SIZE, rankOf(sq) * TILE_SIZE));

            // add visual piece to list to be continuously rendered
            visualPieces.push_back(std::move(vp));
        }
    }

    // sync legalMovesBySq map
    legalMovesBySrcSq = std::vector<std::vector<Square>>(NO_SQ);
    std::vector<Move> legalMoves = game.getLegalMoves();
    for (const Move &move : legalMoves) {
        legalMovesBySrcSq[move.getFromSquare()].push_back(move.getToSquare());
    }
}

Square GuiFrontend::squareUnderMouse() const {
    return xyToSquare(mousePos.x / TILE_SIZE, mousePos.y / TILE_SIZE);
}

void GuiFrontend::update() {
    if (promotionMenu.isVisible) {
        promotionMenu.update(arrowCursor, handCursor, window);
        Piece promotion = promotionMenu.getPromotionPiece();
        if (promotion != NO_PIECE) {
            candidate.setPromotedPieceType(pieceToPT(promotion));

            // apply move to internal game state
            game.makeHumanMove(candidate);

            // update the GUI with this move
            syncPositionToGUI();

            // hide promotionMenu
            promotionMenu.isVisible = false;

            // check if game has ended
            if (game.isGameOver()) {
                game.handleEnd();
            } else {
                std::cout << COLOR_NAMES[game.getSideToMove()] << " to move" << std::endl;
            }
        }

        // pause normal updates when promotion menu is open
        return;
    }

    if (game.getHumanSide() != game.getSideToMove() && !game.isGameOver()) {
        // TODO: get move from engine in a separate worker thread so that this thread can handle
        // GUI updates
        game.makeAIMove();

        // update sprites with this move
        syncPositionToGUI();

        // check if game has ended
        if (game.isGameOver()) {
            game.handleEnd();
            window.close();
        }
    }

    // mouse hovers over a piece
    const Square underMouse = squareUnderMouse();
    const Piece hoveredPiece = game.getPosition().pieceAt(underMouse);
    if (hoveredPiece != NO_PIECE) {
        window.setMouseCursor(handCursor);
    } else {
        window.setMouseCursor(arrowCursor);
    }

    // drag-and-drop logic: move piece with mouse
    if (isDragging && selectedPiece && pieceColor(selectedPiece->piece) == game.getHumanSide()) {
        selectedPiece->sprite->setPosition(
            sf::Vector2f(mousePos.x - 0.5 * TILE_SIZE, mousePos.y - 0.5 * TILE_SIZE));
    }

    // move selection logic
    if (mouseButtonStatus == LMB_DOWN || mouseButtonStatus == LMB_UP) {
        // cancel dragging
        if (mouseButtonStatus == LMB_UP) {
            if (selectedPiece) {
                selectedPiece->snapToSquare(selectedPiece->sq);
            }
            isDragging = false;
        }

        // drag-and-drop logic: select piece with mouse
        auto it = std::find_if(visualPieces.begin(), visualPieces.end(),
                               [underMouse](const VisualPiece &vp) { return vp.sq == underMouse; });
        if (it != visualPieces.end() && mouseButtonStatus == LMB_DOWN && !isDragging) {
            // https://stackoverflow.com/questions/743055/convert-iterator-to-pointer
            selectedPiece = &*it;
            isDragging = true;
        }

        // clicked our own pieces
        if (hoveredPiece != NO_PIECE && pieceColor(hoveredPiece) == game.getHumanSide()) {
            selectedSq = underMouse;
        }

        // clicked a destination square
        if (selectedSq != NO_SQ && selectedSq != underMouse) {
            candidate = buildCandidateMove(selectedSq, underMouse);
            if (candidate != Move::none()) {
                // show promotion menu if pawn is promoting
                if (candidate.isPromotion()) {
                    promotionMenu.show(fileOf(candidate.getToSquare()));
                    return;
                }

                // apply move to internal game state
                game.makeHumanMove(candidate);

                // update the GUI with this move
                syncPositionToGUI();

                // check if game has ended
                if (game.isGameOver()) {
                    game.handleEnd();
                } else {
                    std::cout << COLOR_NAMES[game.getSideToMove()] << " to move" << std::endl;
                }
            } else {
                // reset this piece's position
                selectedPiece->snapToSquare(selectedSq);
            }
            selectedPiece = nullptr;
            selectedSq = NO_SQ;
        }
    }
}

void GuiFrontend::render() {
    // draw the board
    window.clear(sf::Color(50, 50, 50));
    window.draw(boardSprite);

    // draw the pieces
    for (const VisualPiece &vp : visualPieces) {
        window.draw(*vp.sprite);
    }

    // draw the promotionMenu (only visible when open)
    promotionMenu.render(window);

    // display the window
    window.display();
}
