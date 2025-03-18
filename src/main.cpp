#include <iostream>
#include "gui/Game.h"
#include "cli/CliGame.h"

void usage() {
    std::cout << "Usage: ./build/main [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help: Display this message" << std::endl;
    std::cout << "  -c, --cli: Run the game in CLI mode (default)" << std::endl;
    std::cout << "  -g, --gui: Run the game in GUI mode" << std::endl;
}

int main(int argc, char** argv) {
    // parse command line arguments
    bool useCli = true;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            usage();
            return 0;
        } else if (arg == "-c" || arg == "--cli") {
            useCli = true;
        } else if (arg == "-g" || arg == "--gui") {
            useCli = false;
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            usage();
            return 1;
        }
    }

    // run the CLI or GUI
    if (useCli) {
        CliGame game;
        game.run();
    } else {
        Game game;
        game.run();
    }


    return 0;
}