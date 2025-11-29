#include <iostream>

#include "constants.h"
#include "Gui.h"

Gui::Gui() : Gui(defaultFEN, 8) {}

Gui::Gui(const std::string &fen, int depth)
    : Game(fen, depth),
      window(sf::VideoMode({BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE}), "Cheese", sf::Style::Resize),
      view(sf::Vector2f(0, 0), sf::Vector2f(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE)),
      boardSprite(boardTexture)
{
    window.setView(view);

    // load textures
    loadPieceTextures();
    if (!boardTexture.loadFromFile(BOARD_TEXTURE_PATH))
    {
        std::cerr << "Error loading chessboard texture" << std::endl;
    }

    promotionMenu = PromotionMenu(pieceTheme, state.whiteToMove);

    // finalize board and piece sprites
    boardSprite.setTexture(boardTexture);
    pieces = fenToPieces(fen);
}

Gui::Gui(const std::string &fen, int depth, bool playerIsWhite)
    : Game(fen, depth, playerIsWhite),
      window(sf::VideoMode({BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE}), "Cheese", sf::Style::Resize),
      view(sf::Vector2f(0, 0), sf::Vector2f(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE)),
      boardSprite(boardTexture)
{

    window.setView(view);

    // load textures
    loadPieceTextures();
    if (!boardTexture.loadFromFile(BOARD_TEXTURE_PATH))
    {
        std::cerr << "Error loading chessboard texture" << std::endl;
    }

    promotionMenu = PromotionMenu(pieceTheme, state.whiteToMove);

    // finalize board and piece sprites
    boardSprite.setTexture(boardTexture);
    pieces = fenToPieces(fen);
}

void Gui::loadPieceTextures()
{
    for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK})
    {
        if (!pieceTextures[p].loadFromFile(PIECE_TEXTURE_PATH + pieceTheme + pieceFilenames[p] + ".png"))
        {
            std::cerr << "Error loading piece texture: " << pieceFilenames[p] << std::endl;
        }
    }
}

std::list<Piece> Gui::fenToPieces(const std::string &fen)
{
    std::list<Piece> pieces;

    // get position data from FEN
    size_t i = 0;
    int x = 0, y = 0;
    for (; i < fen.size(); i++)
    {
        const char c = fen[i];
        if (c == '/')
        { // move to next row
            x = 0;
            y++;
        }
        else if (isdigit(c))
        { // empty square; skip x squares
            x += c - '0';
        }
        else if (c == ' ')
        { // end of board
            break;
        }
        else
        { // piece
            // rotate 180 degrees if player is black
            int displayX = playerIsWhite ? x : 7 - x;
            int displayY = playerIsWhite ? y : 7 - y;

            // set this piece's bit at the correct position
            PieceType label = fenPieceMap.at(c);
            Piece piece(label, {displayX, displayY}, pieceTextures[label]);
            piece.sprite->setPosition(sf::Vector2f(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE));
            pieces.push_back(std::move(piece));
            x++;
        }
    }
    return pieces;
}

void Gui::run()
{
    while (window.isOpen())
    {
        if (!playersTurn)
        {
            // get move from engine
            Move move = cpu.getMove(state, legalMoves);

            int displayX = move.to.x;
            int displayY = move.to.y;

            // render move to screen
            for (Piece &p : pieces)
            {
                if (p.position == move.from)
                {
                    p.position = move.to;
                    if (p.sprite)
                    {
                        p.sprite->setPosition(sf::Vector2f(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE));
                    }
                    break;
                }
            }

            // remove captured piece from display list
            if (move.capturedPiece != None)
            {
                auto it = std::find_if(pieces.begin(), pieces.end(), [move](const Piece &p)
                                       { return p.type != move.piece && p.position == move.to; });
                pieces.erase(it);
            }

            // handle pawn promotion
            if (move.promotionPiece != None)
            {
                auto it = std::find_if(pieces.begin(), pieces.end(), [move](const Piece &p)
                                       { return p.type == move.piece && p.position == move.to; });
                it->type = move.promotionPiece;
                if (it->sprite)
                {
                    it->sprite->setTexture(pieceTextures[move.promotionPiece]);
                }
            }

            // update state
            state.makeMove(move);
            state.md.history.push_back(state.hash());
            state.print();
            playersTurn = true;

            // get new legal moves for the next turn
            legalMoves = state.generateMoves();

            // check if game has ended
            if (state.isTerminal())
            {
                if (legalMoves.empty())
                {
                    if (state.isCheck())
                    {
                        std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!" << std::endl;
                    }
                    else
                    {
                        std::cout << "Stalemate!" << std::endl;
                    }
                }
                else
                {
                    std::cout << "Draw!" << std::endl;
                }
                window.close();
            }
            else
            {
                state.whiteToMove ? std::cout << "White to move" << std::endl : std::cout << "Black to move" << std::endl;
            }
        }

        handleEvents();
        update();
        render();
    }
}

void Gui::handleEvents()
{
    mousePos = sf::Mouse::getPosition(window);

    // SFML 3.0 event handling
    while (auto event = window.pollEvent())
    {
        // user quits
        if (event->is<sf::Event::Closed>())
            window.close();

        // resize window
        if (const auto *resized = event->getIf<sf::Event::Resized>())
        {
            float windowWidth = resized->size.x;
            float windowHeight = resized->size.y;

            // compute the new view size; ensure the board remains square
            float scale = std::min(windowWidth / BOARD_PIXEL_SIZE, windowHeight / BOARD_PIXEL_SIZE);
            float newWidth = scale * BOARD_PIXEL_SIZE, newHeight = scale * BOARD_PIXEL_SIZE;

            // center board viewport wrt the window
            view.setSize(sf::Vector2f(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
            view.setViewport(sf::FloatRect(
                sf::Vector2f((windowWidth - newWidth) / (2.0f * windowWidth),
                             (windowHeight - newHeight) / (2.0f * windowHeight)),
                sf::Vector2f(newWidth / windowWidth, newHeight / windowHeight)));

            window.setView(view);
        }

        // if promotion menu is open, process its events exclusively
        if (promotionMenu.isVisible)
        {
            auto callback = [this](const PieceType p)
            {
                // update the selected piece to the promoted piece
                selectedPiece->type = p;
                if (selectedPiece->sprite)
                {
                    selectedPiece->sprite->setTexture(pieceTextures[p]);
                }

                candidate.promotionPiece = p;

                std::cout << candidate.toString() << std::endl;

                // apply move to internal game state
                state.makeMove(candidate);
                state.md.history.push_back(state.hash());
                playersTurn = false;
                state.print();

                // get new legal moves for the next turn
                legalMoves = state.generateMoves();

                // check if game has ended
                if (state.isTerminal())
                {
                    if (legalMoves.empty())
                    {
                        if (state.isCheck())
                        {
                            std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!" << std::endl;
                        }
                        else
                        {
                            std::cout << "Stalemate!" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Draw!" << std::endl;
                    }
                    window.close();
                }
                else
                {
                    state.whiteToMove ? std::cout << "White to move" << std::endl : std::cout << "Black to move" << std::endl;
                }

                std::cout << "Promoted to piece " << pieceFilenames[p] << std::endl;
            };

            promotionMenu.handleEvents(window, callback);
            return; // prevent normal game event handling
        }

        // piece release
        if (const auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
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
                        std::cout << m.toString() << std::endl;
                        if (candidate.equals(m))
                        {
                            candidate = m;
                            validMove = true;
                            break;
                        }
                    }

                    std::cout << "[" << (validMove ? "V" : "Inv") << "alid Move] " << candidate.toString() << std::endl;

                    if (validMove) {
                        // remove captured piece from display list
                        if (candidate.capturedPiece != None) {
                            auto it = std::find_if(pieces.begin(), pieces.end(), [this](const Piece &p)
                                                   { return p.type != candidate.piece && p.position.x == candidate.to.x && p.position.y == candidate.to.y; });

                            pieces.erase(it);
                            std::cout << pieceFilenames[it->type] << "on (" << it->position.x << ", " << it->position.y << ") captured" << std::endl;
                        }

                        // show promotion menu if pawn is promoting
                        if (pawnPromoting) {
                            promotionMenu.show(displayX);

                            // snap to grid
                            selectedPiece->position = {displayX, displayY};
                            if (selectedPiece->sprite)
                            {
                                selectedPiece->sprite->setPosition(sf::Vector2f(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE));
                            }

                            return;
                        }

                        // apply move to internal game state
                        state.makeMove(candidate);
                        state.md.history.push_back(state.hash());
                        state.print();
                        playersTurn = false;

                        // get new legal moves for the next turn
                        legalMoves = state.generateMoves();

                        // check if game has ended
                        if (state.isTerminal())
                        {
                            if (state.isCheck())
                            {
                                std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!" << std::endl;
                            }
                            else
                            {
                                std::cout << "Draw!" << std::endl;
                            }
                            window.close();
                        }
                        else
                        {
                            state.whiteToMove ? std::cout << "White to move" << std::endl : std::cout << "Black to move" << std::endl;
                        }
                    }
                    else
                    {
                        // reset piece position if move is invalid
                        newX = oldX;
                        newY = oldY;
                        displayX = playerIsWhite ? newX : 7 - newX;
                        displayY = playerIsWhite ? newY : 7 - newY;
                    }

                    // update this piece's position
                    selectedPiece->position = {displayX, displayY};
                    if (selectedPiece->sprite)
                    {
                        selectedPiece->sprite->setPosition(sf::Vector2f(displayX * TILE_PIXEL_SIZE, displayY * TILE_PIXEL_SIZE));
                    }
                    selectedPiece = nullptr;
                }
            }
        }
    }
}

void Gui::update()
{
    // pause normal updates when promotion menu is open
    if (promotionMenu.isVisible)
        return;

    // mouse hovers over a piece
    for (auto &piece : pieces)
    {
        if (piece.sprite && piece.sprite->getGlobalBounds().contains(sf::Vector2f(mousePos.x, mousePos.y)))
        {

            // drag-and-drop logic: select piece with mouse
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !isDragging)
            {
                selectedPiece = &piece;
                isDragging = true;
            }
        }
    }

    // drag-and-drop logic: move piece with mouse
    if (isDragging && selectedPiece && selectedPiece->sprite)
    {
        selectedPiece->sprite->setPosition(sf::Vector2f(mousePos.x - TILE_PIXEL_SIZE / 2, mousePos.y - TILE_PIXEL_SIZE / 2));
    }
}

void Gui::render()
{
    window.clear(sf::Color(50, 50, 50));
    window.draw(boardSprite);
    for (const auto &p : pieces)
    {
        if (p.sprite)
        {
            window.draw(*p.sprite);
        }
    }
    promotionMenu.render(window);
    window.display();
}