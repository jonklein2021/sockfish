#include <iostream>

#include "constants.h"
#include "Gui.h"

Gui::Gui() : Gui(defaultFEN, 8) {}

Gui::Gui(const std::string &fen, int depth)
    : Game(fen, depth),
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

Gui::Gui(const std::string &fen, int depth, bool playerIsWhite)
    : Game(fen, depth, playerIsWhite),
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

    // get position data from FEN
    size_t i = 0;
    int x = 0, y = 0;
    for (; i < fen.size(); i++) {
        const char c = fen[i];
        if (c == '/') { // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) { // empty square; skip x squares
            x += c - '0';
        } else if (c == ' ') { // end of board
            break;
        } else { // piece
            // rotate 180 degrees if player is black
            int displayX = playerIsWhite ? x : 7 - x;
            int displayY = playerIsWhite ? y : 7 - y;

            // set this piece's bit at the correct position
            PieceType label = fenPieceMap.at(c);
            Piece piece(label, {displayX, displayY}, sf::Sprite(pieceTextures[label]));
            piece.sprite.setPosition(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE);
            pieces.push_back(piece);
            x++;
        }
    }
    return pieces;
}

void Gui::run() {
    while (window.isOpen()) {
        if (!playersTurn) {
            // get move from engine
            Move move = cpu.getMove(state, legalMoves);

            // int displayX = playerIsWhite ? move.to.x : 7 - move.to.x;
            // int displayY = playerIsWhite ? move.to.y : 7 - move.to.y;
            int displayX = move.to.x;
            int displayY = move.to.y;

            // render move to screen
            for (Piece& p : pieces) {
                if (p.position == move.from) {
                    p.position = move.to;
                    p.sprite.setPosition(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE);
                    break;
                }
            }

            // remove captured piece from display list
            if (move.capturedPiece != None) {
                auto it = std::find_if(pieces.begin(), pieces.end(), [this, move](const Piece& p){
                    return p.type != move.piece && p.position == move.to;
                });
                pieces.erase(it);
            }

            // handle pawn promotion
            if (move.promotionPiece != None) {
                auto it = std::find_if(pieces.begin(), pieces.end(), [this, move](const Piece& p){
                    return p.type == move.piece && p.position == move.to;
                });
                it->type = move.promotionPiece;
                it->sprite.setTexture(pieceTextures[move.promotionPiece]);
            }
            
            // update state
            state.makeMove(move);
            state.print();
            playersTurn = true;

            // get new legal moves for the next turn
            legalMoves = state.generateMoves();
            for (const Move& m : legalMoves) {
                std::cout << m.to_string() << std::endl;
            }

        }

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
            auto callback = [this](const PieceType p) {
                // update the selected piece to the promoted piece
                selectedPiece->type = p;
                selectedPiece->sprite.setTexture(pieceTextures[p]);

                candidate.promotionPiece = p;

                std::cout << candidate.to_string() << std::endl;

                // apply move to internal game state
                state.makeMove(candidate);
                playersTurn = false;
                state.print();

                // get new legal moves for the next turn
                legalMoves = state.generateMoves();

                // check if game has ended
                if (state.isTerminal()) {
                    if (state.isCheck()) {
                        std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!" << std::endl;
                    } else {
                        std::cout << "Stalemate!" << std::endl;
                    }
                    window.close();
                } else {   
                    state.whiteToMove ? std::cout << "White to move" << std::endl : std::cout << "Black to move" << std::endl;
                }

                std::cout << "Promoted to piece " << pieceFilenames[p] << std::endl;
            };

            promotionMenu.handleEvents(window, callback);
            return; // prevent normal game event handling
        }

        // piece release
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDragging = false;
            if (playersTurn && selectedPiece) {
                int oldX = selectedPiece->position.x, oldY = selectedPiece->position.y;
                
                // snap to nearest tile
                int newX = mousePos.x / TILE_PIXEL_SIZE, newY = mousePos.y / TILE_PIXEL_SIZE;
                
                // rotate 180 degrees if player is black
                int displayX = playerIsWhite ? newX : 7 - newX;
                int displayY = playerIsWhite ? newY : 7 - newY;

                bool pawnPromoting = (selectedPiece->type == WP && newY == 0) || (selectedPiece->type == BP && newY == 7);

                // create move object, isCapture and promotionPiece will be overridden later
                candidate = Move({oldX, oldY}, {newX, newY}, selectedPiece->type, None, None);

                // necessary to match with a legal move
                if (pawnPromoting)
                    candidate.promotionPiece = state.whiteToMove ? WQ : BQ;

                // check this move against set of legal moves
                bool validMove = false;
                std::cout << "LEGAL MOVES:" << std::endl;
                for (const Move &m : legalMoves) {
                    std::cout << m.to_string() << std::endl;
                    if (candidate.equals(m)) {
                        candidate = m;
                        validMove = true;
                        break;
                    }
                }

                // this line is raw as hell
                std::cout << "[" << (validMove ? "V" : "Inv") << "alid Move] " << candidate.to_string() << std::endl;
                
                if (validMove) {
                    // remove captured piece from display list
                    if (candidate.capturedPiece != None) {
                        auto it = std::find_if(pieces.begin(), pieces.end(), [this](const Piece& p){
                            return p.type != candidate.piece && p.position.x == candidate.to.x && p.position.y == candidate.to.y;
                        });
                        
                        pieces.erase(it);
                        std::cout << pieceFilenames[it->type] << "on (" << it->position.x << ", " << it->position.y << ") captured" << std::endl;
                    }


                    // show promotion menu if pawn is promoting
                    if (pawnPromoting) {

                        promotionMenu.show(displayX);
                        window.setMouseCursor(arrowCursor);
    
                        // snap to grid
                        selectedPiece->position = {displayX, displayY};
                        selectedPiece->sprite.setPosition(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE);
                        
                        // stop further processing for now;
                        // handle the rest in callback lambda to promotionMenu.handleEvents()
                        return;
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
                    state.makeMove(candidate);
                    state.print();
                    playersTurn = false;

                    // get new legal moves for the next turn
                    legalMoves = state.generateMoves();

                    // check if game has ended
                    if (state.isTerminal()) {
                        if (state.isCheck()) {
                            std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!" << std::endl;
                        } else {
                            std::cout << "Draw!" << std::endl;
                        }
                        window.close();
                    } else {   
                        state.whiteToMove ? std::cout << "White to move" << std::endl : std::cout << "Black to move" << std::endl;
                    }
                    
                } else {
                    // reset piece position if move is invalid
                    newX = oldX;
                    newY = oldY;
                    displayX = playerIsWhite ? newX : 7 - newX;
                    displayY = playerIsWhite ? newY : 7 - newY;
                }

                // update this piece's position
                selectedPiece->position = {displayX, displayY};
                selectedPiece->sprite.setPosition(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE);
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

            // drag-and-drop logic: select piece with mouse
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !isDragging) {
                selectedPiece = &piece;
                isDragging = true;
            }
        }
    }

    // update mouse cursor if hovering over piece
    window.setMouseCursor(hoveringOverPiece ? handCursor : arrowCursor);
    
    // drag-and-drop logic: move piece with mouse
    if (isDragging && selectedPiece) {
        selectedPiece->sprite.setPosition(mousePos.x - TILE_PIXEL_SIZE / 2, mousePos.y - TILE_PIXEL_SIZE / 2);
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
