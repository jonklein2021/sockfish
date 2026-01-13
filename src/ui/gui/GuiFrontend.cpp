#include "GuiFrontend.h"

#include "src/bitboard/bit_tools.h"
#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <algorithm>
#include <iostream>
#include <vector>

GuiFrontend::GuiFrontend(GameController &game)
    : game(game),
      window(sf::VideoMode({BOARD_SIZE, BOARD_SIZE}), "Cheese", sf::Style::Resize),
      view({BOARD_SIZE / 2.f, BOARD_SIZE / 2.f}, {BOARD_SIZE, BOARD_SIZE}),
      themeName(DEFAULT_PIECE_THEME),
      boardTexture(BOARD_TEXTURE_PATH),
      boardSprite(boardTexture),
      arrowCursor(sf::Cursor::Type::Arrow),
      handCursor(sf::Cursor::Type::Hand),
      promotionMenu(std::string(DEFAULT_PIECE_THEME), game.getHumanSide()) {
    arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();
    handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value();
    initializeScreen();
    syncPositionToGUI();
}

void GuiFrontend::initializeScreen() {
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

    // set up cursors
    arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value();
    handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand).value();
    window.setMouseCursor(arrowCursor);
}

void GuiFrontend::run() {
    while (window.isOpen()) {
        if (game.getHumanSide() != game.getSideToMove()) {
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

        // getting player's move is handled here
        handleEvents();
        update();
        render();
    }
}

Move GuiFrontend::buildCandidateMove(const VisualPiece *piece, Square dst) const {
    Square src = piece->sq;

    // Castling handling
    if (pieceToPT(piece->piece) == KING) {
        if ((src == e1 && dst == g1) || (src == e8 && dst == g8)) {
            return MoveGenerator::createCastlingMove(false, game.getHumanSide());
        }
        if ((src == e1 && dst == c1) || (src == e8 && dst == c8)) {
            return MoveGenerator::createCastlingMove(true, game.getHumanSide());
        }
    }

    return Move(src, dst);
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
    Move candidate;
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

        // if promotion menu is open, process its events exclusively
        if (promotionMenu.isVisible) {
            auto callback = [&](const Piece p) {
                // update the selected piece to the promoted piece
                // N.B: texture change should be handled by boardRenderer
                selectedPiece->piece = p;

                candidate.setPromotedPieceType(pieceToPT(p));

                // apply move to internal game state
                game.makeHumanMove(candidate);

                // check if game has ended
                if (game.isGameOver()) {
                    game.handleEnd();
                    window.close();
                }

                std::cout << "Promoted to " << PIECE_NAMES[p] << std::endl;
            };

            promotionMenu.handleEvents(window, callback);
            return;  // prevent normal game event handling
        }

        // piece release
        if (const auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                isDragging = false;

                // DEBUG
                printf("Clicked square %s, mousePos=(%.2f, %.2f)\n",
                       squareToCoordinateString(squareUnderMouse()).c_str(), mousePos.x,
                       mousePos.y);

                if (game.getHumanSide() == game.getSideToMove() && selectedPiece &&
                    pieceColor(selectedPiece->piece) == game.getHumanSide()) {
                    // snap to nearest tile
                    int newX = mousePos.x / TILE_SIZE, newY = mousePos.y / TILE_SIZE;

                    Square srcSq = selectedPiece->sq;
                    Square dstSq = xyToSquare(newX, newY);

                    // TODO: rotate 180 degrees if player is black

                    bool pawnPromoting = (selectedPiece->piece == WP && newY == 0) ||
                                         (selectedPiece->piece == BP && newY == 7);

                    candidate = buildCandidateMove(selectedPiece, dstSq);

                    // check this move against set of legal moves
                    candidate = validateMove(candidate);

                    std::cout << "[" << (candidate != Move::none() ? "V" : "Inv") << "alid Move] "
                              << candidate.toString() << std::endl;

                    if (candidate != Move::none()) {
                        // show promotion menu if pawn is promoting
                        if (pawnPromoting) {
                            promotionMenu.show(newX);

                            // snap to grid
                            selectedPiece->sq = dstSq;
                            if (selectedPiece->sprite) {
                                selectedPiece->sprite->setPosition(
                                    sf::Vector2f(newX * TILE_SIZE, newY * TILE_SIZE));
                            }

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
                            std::cout << COLOR_NAMES[game.getSideToMove()] << " to move"
                                      << std::endl;
                        }
                    } else {
                        // reset this piece's position
                        selectedPiece->sprite->setPosition(
                            sf::Vector2f(fileOf(srcSq) * TILE_SIZE, rankOf(srcSq) * TILE_SIZE));
                    }

                    selectedPiece = nullptr;
                }
            }
        }
    }
}

void GuiFrontend::syncPositionToGUI() {
    // reset visualPieces to match the current position
    visualPieces.clear();
    for (Piece p : ALL_PIECES) {
        auto bb = game.getPosition().getPieceBB(p);
        while (bb) {
            const Square pieceSq = Square(getLsbIndex(bb));

            VisualPiece vp(p, pieceSq, pieceTextures[p]);
            vp.sprite->setPosition(
                sf::Vector2f(fileOf(pieceSq) * TILE_SIZE, rankOf(pieceSq) * TILE_SIZE));

            // add visual piece to list to be continuously rendered
            visualPieces.push_back(std::move(vp));

            // pop LSB
            bb &= bb - 1;
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
    // pause normal updates when promotion menu is open
    if (promotionMenu.isVisible) {
        return;
    }

    // mouse hovers over a piece
    const Square hovered = squareUnderMouse();
    auto it = std::find_if(visualPieces.begin(), visualPieces.end(),
                           [hovered](const VisualPiece &vp) { return vp.sq == hovered; });
    if (it != visualPieces.end() && it->sprite &&
        it->sprite->getGlobalBounds().contains(sf::Vector2f(mousePos.x, mousePos.y))) {
        window.setMouseCursor(handCursor);

        // drag-and-drop logic: select piece with mouse
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !isDragging) {
            // https://stackoverflow.com/questions/743055/convert-iterator-to-pointer
            selectedPiece = &*it;
            isDragging = true;
        }
    } else {
        window.setMouseCursor(arrowCursor);
    }

    // drag-and-drop logic: move piece with mouse
    if (isDragging && selectedPiece && pieceColor(selectedPiece->piece) == game.getHumanSide() &&
        selectedPiece->sprite) {
        selectedPiece->sprite->setPosition(
            sf::Vector2f(mousePos.x - 0.5 * TILE_SIZE, mousePos.y - 0.5 * TILE_SIZE));
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

    promotionMenu.render(window);
    window.display();
}
