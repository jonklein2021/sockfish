#include <iostream>

#include "constants.h"
#include "game/cli/Cli.h"
#include "game/gui/Gui.h"

void usage() {
    std::cout << "Usage: ./build/main [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help      Display this message" << std::endl;
    std::cout << "  -c, --cli       Run the game in CLI mode (default)" << std::endl;
    std::cout << "  -g, --gui       Run the game in GUI mode" << std::endl;
    std::cout << "  -f  <string>    Specify a custom FEN string for the starting position" << std::endl;

}

int main(int argc, char** argv) {
    // parse command line arguments
    bool useCli = true;
    std::string fen = defaultFEN;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            usage();
            return 0;
        } else if (arg == "-c" || arg == "--cli") {
            useCli = true;
        } else if (arg == "-g" || arg == "--gui") {
            useCli = false;
        } else if (arg == "-f") {
            if (i + 1 < argc) {
                fen = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -f option requires a FEN string" << std::endl;
                usage();
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            usage();
            return 1;
        }
    }

    std::cout << "Initializing the " << (useCli ? "CLI" : "GUI") << " with FEN string: " << fen << std::endl;

    // run the CLI or GUI
    Game *game;
    if (useCli) {
        game = new Cli(fen);
    } else {
        game = new Gui(fen);
    }
    game->run();

    return 0;
}