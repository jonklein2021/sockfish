#pragma once

#include <ctime>
#include <memory>

#include "Engine.h"
#include "GameState.h"

/**
 * Base class for a chess game that
 * classes in Cli.h and Gui.h extend.
 */
class Game {
protected:
    Engine cpu;
    GameState state;
    std::vector<Move> legalMoves;
    bool playerIsWhite = true;
    bool playersTurn = true;

public:
    /** Default constructor — only Builder may access it */
    Game() = default;

    virtual ~Game() = default;

    /**
     * Runs the game loop
     */
    virtual void run() = 0;

    /**
     * Builder for Game
     */
    class Builder {
    private:
        std::string fen = defaultFEN;
        int depth = 8;
        bool randomizePlayerColor = true;
        bool explicitPlayerIsWhite = true;
        bool playerColorWasSet = false;

    public:
        Builder() = default;

        Builder& withFEN(const std::string& f) {
            fen = f;
            return *this;
        }

        Builder& withDepth(int d) {
            depth = d;
            return *this;
        }

        Builder& withPlayerIsWhite(bool isWhite) {
            explicitPlayerIsWhite = isWhite;
            playerColorWasSet = true;
            randomizePlayerColor = false;
            return *this;
        }

        Builder& withRandomizePlayerColorEnabled(bool enabled = true) {
            randomizePlayerColor = enabled;
            return *this;
        }

        /**
         * Build a Game instance.
         * Because Game is abstract, derived classes typically call this from their own builders.
         */
        std::unique_ptr<Game> build() {
            auto g = std::unique_ptr<Game>(new Game());

            // seed RNG
            std::srand(std::time(nullptr));

            // engine / state construction
            g->cpu = Engine(depth);
            g->state = GameState(fen);
            g->legalMoves = g->state.generateMoves();

            // color selection
            if (randomizePlayerColor && !playerColorWasSet) {
                g->playerIsWhite = (std::rand() % 2) & 1;
            } else {
                g->playerIsWhite = explicitPlayerIsWhite;
            }

            // push initial hash
            g->state.md.history.push_back(g->state.hash());

            // whose turn?
            g->playersTurn = (g->state.whiteToMove == g->playerIsWhite);

            return g;
        }
    };
};
