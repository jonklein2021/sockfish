#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "constants.h"
#include "game.h"
#include "piece.h"

// TODO: Move these to header files

std::string pieceTheme = "horsey/";

// directories to assets
const char* PIECE_TEXTURE_PATH = "assets/pieces/";
const char* BOARD_TEXTURE_PATH = "assets/board.png";

std::unordered_map <char, PieceType> fenPieceMap = {
    {'P', WP}, {'N', WN}, {'B', WB}, {'R', WR}, {'Q', WQ}, {'K', WK},
    {'p', BP}, {'n', BN}, {'b', BB}, {'r', BR}, {'q', BQ}, {'k', BK}
};

// FEN to Piece list conversion
std::list<Piece> fenToPieces(const std::string& fen, std::unordered_map<PieceType, sf::Texture>& textures) {
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
            piece.isWhite = isupper(c);
            piece.type = fenPieceMap[c];
            piece.position = {x, y};
            piece.sprite = sf::Sprite(textures[piece.type]);
            piece.sprite.setPosition(x * 64, y * 64);
            piece.isAlive = true;
            pieces.push_back(piece);
            x++;
        }
    }
    return pieces;
}

int main() {
    // get cursors
    sf::Cursor arrowCursor, handCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);
    handCursor.loadFromSystem(sf::Cursor::Hand);

    // create window
    sf::RenderWindow window(sf::VideoMode(BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE), "Cheess", sf::Style::Resize);

    // load board texture
    sf::Texture boardTexture;
    if (!boardTexture.loadFromFile(BOARD_TEXTURE_PATH)) {
        std::cerr << "Error loading chessboard texture!" << std::endl;
        return -1;
    }

    // create board sprite from texture
    sf::Sprite boardSprite(boardTexture);
    
    // create a default square view for the board
    sf::View view(sf::FloatRect(0, 0, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE));
    window.setView(view);

    // map to each piece to its texture
    std::unordered_map<PieceType, sf::Texture> pieceTextures;
    for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
        sf::Texture texture;
        if (!texture.loadFromFile(PIECE_TEXTURE_PATH + pieceTheme + pieceNames[p] + ".png")) {
            std::cerr << "Error loading piece texture: " << pieceNames[p] << std::endl;
            return -1;
        }
        pieceTextures[p] = texture;
    }

    // generate piece structs from FEN (includes sprite)
    std::list<Piece> pieces = fenToPieces(startingFen, pieceTextures);
    
    // used in loop as drag-and-drop variables
    Piece* selectedPiece = nullptr;
    bool isDragging = false;
    
    // used by backend to analyze the game state
    GameState state(fenToBitBoard(startingFen), 0ull, 0ull, true, false, false, false, false, false, false);

    // main render loop typeshit
    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // detect if mouse is hovering over a piece
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

        // event handler loop
        while (window.pollEvent(event)) {
            // window close
            if (event.type == sf::Event::Closed)
                window.close();

            // left-click release
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                isDragging = false;
                if (selectedPiece) {
                    // use to snap to nearest tile
                    int oldX = selectedPiece->position.x, oldY = selectedPiece->position.y;
                    int newX = mousePos.x / TILE_PIXEL_SIZE, newY = mousePos.y / TILE_PIXEL_SIZE;

                    // create move object, isCapture arg doesn't matter yet because the
                    // backend will override it with the correct value
                    Move candidate({oldX, oldY}, {newX, newY}, selectedPiece->type, false);

                    // check this move against set of legal moves
                    std::vector<Move> legalMoves = generateMoves(state);
                    bool validMove = false;
                    for (const Move &m : legalMoves) {
                        std::cout << to_string(m) << std::endl;
                        if (candidate.equals(m)) {
                            candidate = m;
                            validMove = true;
                            // break; // uncomment this when not testing
                        }
                    }

                    // this line is raw as hell
                    std::cout << "[" << (validMove ? "V" : "Inv") << "alid Move] " << to_string(candidate) << std::endl;
                    
                    if (validMove) {
                        // update frontend properties
                        if (candidate.isCapture) {
                            auto it = std::find_if(pieces.begin(), pieces.end(), [candidate](const Piece& p){
                                return p.type != candidate.pieceMoved && p.position.x == candidate.to.x && p.position.y == candidate.to.y;
                            });
                            
                            pieces.erase(it);
                            std::cout << pieceNames[it->type] << "(" << it->position.x << ", " << it->position.y << ") captured" << std::endl;
                            
                        }

                        
                        // update game state
                        state.whiteToMove = !state.whiteToMove;
                        state.ApplyMove(candidate);
                        
                    } else {
                        // reset piece position if move is invalid
                        newX = oldX;
                        newY = oldY;
                    }
                    
                    selectedPiece->position = {newX, newY};
                    // selectedPiece->type = candidate.pieceMoved;
                    
                    // update sprite position, remembering to snap to a tile
                    selectedPiece->sprite.setPosition(newX * TILE_PIXEL_SIZE, newY * TILE_PIXEL_SIZE);
                    selectedPiece = nullptr;
                    
                    std::cout << "Remaining pieces:" << std::endl;
                    for (Piece p : pieces) {
                        std::cout << pieceNames[p.type] << "(" << p.position.x << ", " << p.position.y << ")" << std::endl;
                    }
                    
                    prettyPrint(state.board);
                }
            }

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
        }

        // drag-and-drop logic; move piece with mouse
        if (isDragging && selectedPiece) {
            selectedPiece->sprite.setPosition(mousePos.x - TILE_PIXEL_SIZE / 2, mousePos.y - TILE_PIXEL_SIZE / 2);
        }

        // render sprites
        window.clear(sf::Color(50, 50, 50));
        window.draw(boardSprite);
        for (const auto& p : pieces) {
            if (p.isAlive)
                window.draw(p.sprite);
        }
        window.display();
    }

    return 0;
}
