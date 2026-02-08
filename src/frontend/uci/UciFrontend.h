#include "src/ai/Engine.h"
#include "src/ai/search/ManualSearchStopper.h"
#include "src/core/Position.h"
#include "src/core/types.h"

class UciFrontend {
   private:
    Position pos;
    Engine engine;
    ManualSearchStopper stopper;

    // search thread variables
    std::atomic<int> searchDepth = -1;

    void searchWorker();

   public:
    UciFrontend()
        : pos(std::string(STARTING_POSITION_FEN)), engine(stopper) {}

    void run();
};
