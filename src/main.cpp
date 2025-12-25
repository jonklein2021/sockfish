#include "CliFrontend.h"
#include "Config.h"
#include "Engine.h"
#include "GameController.h"
#include "GuiFrontend.h"
#include "MoveGenerator.h"

#include <cstdlib>
#include <ctime>
#include <memory>

int main(int argc, char **argv) {
    std::srand(std::time(nullptr));

    // parse command line arguments
    Config cfg;
    cfg.parseArgs(argc, argv);

    // randomly assign colors if no override
    if (cfg.randomColor) {
        cfg.randomizeSides();
    }

    cfg.print();

    // create base objects
    std::shared_ptr<Position> pos = std::make_shared<Position>(cfg.fen);
    std::shared_ptr<MoveGenerator> moveGenerator = std::make_shared<MoveGenerator>(pos);
    std::unique_ptr<Engine> engine = std::make_unique<Engine>(cfg.searchDepth);
    GameController gameController(pos, moveGenerator, std::move(engine), cfg.humanSide);

    // create and launch game
    if (cfg.useGui) {
        GuiFrontend gui(gameController);
        gui.run();
    } else {
        CliFrontend cli(gameController);
        cli.run();
    }

    return 0;
}
