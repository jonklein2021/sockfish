#include <iostream>

#include "constants.h"
#include "game/cli/Cli.h"
#include "game/gui/Gui.h"

void usage() {
    std::ostringstream ss;

    ss << "Usage: ./build/main [options]\n";
    ss << "Options:\n";
    ss << "  -h, --help      Display this message\n";
    ss << "  -c, --cli       Run the game in CLI mode (default)\n";
    ss << "  -g, --gui       Run the game in GUI mode\n";
    ss << "  -w, --white     Play with the white pieces\n";
    ss << "  -b, --black     Play with the black pieces\n";
    ss << "  -f  <string>    Specify a custom FEN string for the starting "
          "position\n";
    ss << "  -d  <depth>     Specify the depth of the minimax search (default: "
          "4)\n";
    ss << "  -t  <theme>     Specify the piece theme for the GUI\n";

    std::cout << ss.str() << std::endl;
}

int main(int argc, char **argv) {
    // parse command line arguments
    bool useCli = true;
    std::string playerColorOverride = ""; // TODO: Make this an enum
    std::string fen = defaultFEN;
    std::string pieceTheme = "horsey/";
    int depth = 4;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            usage();
            return 0;
        } else if (arg == "-c" || arg == "--cli") {
            useCli = true;
        } else if (arg == "-g" || arg == "--gui") {
            useCli = false;
        } else if (arg == "-w" || arg == "--white") {
            playerColorOverride = "white";
        } else if (arg == "-b" || arg == "--black") {
            playerColorOverride = "black";
        } else if (arg == "-t") {
            if (i + 1 < argc) {
                pieceTheme = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -t option requires a theme" << std::endl;
                usage();
                return 1;
            }
        } else if (arg == "-f") {
            if (i + 1 < argc) {
                fen = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -f option requires a FEN string"
                          << std::endl;
                usage();
                return 1;
            }
        } else if (arg == "-d") {
            if (i + 1 < argc) {
                depth = std::stoi(argv[i + 1]);
                i++;
            } else {
                std::cerr << "Error: -d option requires a depth" << std::endl;
                usage();
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            usage();
            return 1;
        }
    }

    bool playerIsWhite = playerColorOverride.empty()
                             ? std::rand() % 2 == 0
                             : (playerColorOverride == "white" ? true : false);

    std::cout << "Initializing game in " << (useCli ? "CLI" : "GUI")
              << " mode...\n";
    std::cout << "  Player color: " << (playerIsWhite ? "white" : "black")
              << (playerColorOverride.empty() ? " (random)"
                                              : playerColorOverride)
              << "\n";
    std::cout << "  FEN: " << fen << "\n";
    std::cout << "  Depth: " << depth << "\n";
    std::cout << "  Piece theme: " << pieceTheme << std::endl;

    // run the CLI or GUI
    std::unique_ptr<Game> game;

    if (useCli) {
        game = std::make_unique<Cli>(
            Cli(std::make_unique<Engine>(Engine(depth)), fen, playerIsWhite));
    } else {
        game = std::make_unique<Gui>(
            Gui(std::make_unique<Engine>(Engine(depth)), fen, playerIsWhite));
    }

    game->run();
    return 0;
}
