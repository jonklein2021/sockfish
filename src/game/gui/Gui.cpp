#include <iostream>

#include "constants.h"
#include "Gui.h"

Gui::Gui() : Game() {}

Gui::Gui(const std::string &fen)
    : Game(fen),
      window(sf::VideoMode(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE), "Cheese", sf::Style::Resize),
      view(sf::FloatRect(0, 0, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE)) {
          
    window.setView(view);

    // load textures
    loadPieceTextures();
    if (!boardTexture.loadFromFile(BOARD_TEXTURE_PATH)) {
        std::cerr << "Error loading chessboard texture" << std::endl;
    }

    promotionMenu = PromotionMenu(pieceTheme, state.whiteToMove);
    
    // finalize board and piece sprites
    boardSprite.setTexture(boardTexture);
    pieces = fenToPieces(fen);

    // load cursors
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);
    handCursor.loadFromSystem(sf::Cursor::Hand);
}

void Gui::loadPieceTextures() {
    for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
        if (!pieceTextures[p].loadFromFile(PIECE_TEXTURE_PATH + pieceTheme + pieceFilenames[p] + ".png")) {
            std::cerr << "Error loading piece texture: " << pieceFilenames[p] << std::endl;
        }
    }
}

std::list<Piece> Gui::fenToPieces(const std::string& fen) {
    std::list<Piece> pieces;

    int x = 0, y = 0;
    for (const char& c : fen) {
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square; skip x squares
            x += c - '0';
        } else { // piece
            Piece piece;
            piece.type = fenPieceMap.at(c);
            piece.position = {x, y};
            piece.sprite = sf::Sprite(pieceTextures[piece.type]);
            piece.sprite.setPosition(x * 64, y * 64);
            pieces.push_back(piece);
            x++;
        }
    }
    return pieces;
}

void Gui::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Gui::handleEvents() {
    sf::Event event;
    mousePos = sf::Mouse::getPosition(window);

    while (window.pollEvent(event)) {
        // user quits
        if (event.type == sf::Event::Closed)
            window.close();
        
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

        // if promotion menu is open, process its events exclusively
        if (promotionMenu.isVisible) {
            promotionMenu.handleEvents(window);
            return; // prevent normal game event handling
        }

        // piece release
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
            if (selectedPiece) {
                int oldX = selectedPiece->position.x, oldY = selectedPiece->position.y;
                
                // snap to nearest tile
                int newX = mousePos.x / TILE_PIXEL_SIZE, newY = mousePos.y / TILE_PIXEL_SIZE;

                // check for pawn promotion
                if ((selectedPiece->type == WP && newY == 0) || (selectedPiece->type == BP && newY == 7)) {
                    promotionMenu.show(newX);
                    selectedPiece = nullptr;
                    return; // stop further processing for now
                }

                // create move object, isCapture arg doesn't matter yet because the
                // backend will override it with the correct value
                Move candidate({oldX, oldY}, {newX, newY}, selectedPiece->type, false);

                // check this move against set of legal moves
                bool validMove = false;
                std::cout << "LEGAL MOVES:" << std::endl;
                for (const Move &m : legalMoves) {
                    std::cout << m.to_string() << std::endl;
                    if (candidate.equals(m)) {
                        candidate = m;
                        validMove = true;
                        // break; // TODO: uncomment this when not testing
                    }
                }

                // this line is raw as hell
                std::cout << "[" << (validMove ? "V" : "Inv") << "alid Move] " << candidate.to_string() << std::endl;
                
                if (validMove) {
                    // remove captured piece from display list
                    if (candidate.isCapture) {
                        auto it = std::find_if(pieces.begin(), pieces.end(), [candidate](const Piece& p){
                            return p.type != candidate.piece && p.position.x == candidate.to.x && p.position.y == candidate.to.y;
                        });
                        
                        pieces.erase(it);
                        std::cout << pieceFilenames[it->type] << "on (" << it->position.x << ", " << it->position.y << ") captured" << std::endl;
                    }

                    // update the rook's position if castling
                    if (candidate.piece == WK && candidate.from.x == 4 && candidate.from.y == 7 && candidate.to.x == 6 && candidate.to.y == 7) {
                        auto it = std::find_if(pieces.begin(), pieces.end(), [](const Piece& p){
                            return p.type == WR && p.position.x == 7 && p.position.y == 7;
                        });
                        it->position = {5, 7};
                        it->sprite.setPosition(5 * TILE_PIXEL_SIZE, 7 * TILE_PIXEL_SIZE);
                    }

                    if (candidate.piece == WK && candidate.from.x == 4 && candidate.from.y == 7 && candidate.to.x == 2 && candidate.to.y == 7) {
                        auto it = std::find_if(pieces.begin(), pieces.end(), [](const Piece& p){
                            return p.type == WR && p.position.x == 0 && p.position.y == 7;
                        });
                        it->position = {3, 7};
                        it->sprite.setPosition(3 * TILE_PIXEL_SIZE, 7 * TILE_PIXEL_SIZE);
                    }

                    if (candidate.piece == BK && candidate.from.x == 4 && candidate.from.y == 0 && candidate.to.x == 6 && candidate.to.y == 0) {
                        auto it = std::find_if(pieces.begin(), pieces.end(), [](const Piece& p){
                            return p.type == BR && p.position.x == 7 && p.position.y == 0;
                        });
                        it->position = {5, 0};
                        it->sprite.setPosition(5 * TILE_PIXEL_SIZE, 0);
                    }

                    if (candidate.piece == BK && candidate.from.x == 4 && candidate.from.y == 0 && candidate.to.x == 2 && candidate.to.y == 0) {
                        auto it = std::find_if(pieces.begin(), pieces.end(), [](const Piece& p){
                            return p.type == BR && p.position.x == 0 && p.position.y == 0;
                        });
                        it->position = {3, 0};
                        it->sprite.setPosition(3 * TILE_PIXEL_SIZE, 0);
                    }

                    // apply move to internal game state
                    state.applyMove(candidate);
                    state.board.prettyPrint();

                    // get new legal moves for the next turn
                    legalMoves = state.generateMoves();

                    // check if game has ended
                    if (legalMoves.empty()) {
                        if (state.isCheck()) {
                            std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!" << std::endl;
                        } else {
                            std::cout << "Stalemate!" << std::endl;
                        }
                        window.close();
                    } else {   
                        state.whiteToMove ? std::cout << "White to move" << std::endl : std::cout << "Black to move" << std::endl;
                    }
                    
                } else {
                    // reset piece position if move is invalid
                    newX = oldX;
                    newY = oldY;
                }

                // update this piece's position
                selectedPiece->position = {newX, newY};
                selectedPiece->sprite.setPosition(newX * TILE_PIXEL_SIZE, newY * TILE_PIXEL_SIZE);
                selectedPiece = nullptr;
            }
        }
    }
}

void Gui::update() {
    // pause normal updates when promotion menu is open
    if (promotionMenu.isVisible) return;

    // mouse hovers over a piece
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
    
    // drag-and-drop logic; move piece with mouse
    if (isDragging && selectedPiece) {
        selectedPiece->sprite.setPosition(mousePos.x - TILE_PIXEL_SIZE / 2, mousePos.y - TILE_PIXEL_SIZE / 2);
    }
    
    PieceType promotedPiece = promotionMenu.getPromotionPiece();

    if (promotedPiece != None) {
        std::cout << "Promoted to piece " << pieceFilenames[promotedPiece] << std::endl;
    }
}

void Gui::render() {
    window.clear(sf::Color(50, 50, 50));
    window.draw(boardSprite);
    for (const auto& p : pieces) {
        window.draw(p.sprite);
    }
    promotionMenu.render(window);
    window.display();
}
