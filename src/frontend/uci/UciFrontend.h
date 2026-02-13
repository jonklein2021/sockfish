#include "src/ai/Engine.h"
#include "src/ai/search/SearchStopwatch.h"
#include "src/core/Position.h"
#include "src/core/types.h"

#include <condition_variable>
#include <mutex>

class UciFrontend {
   private:
    Position pos;
    SearchStopwatch stopper;
    Engine engine;

    // this mutex synchronizes writing to searchDepth
    std::mutex mtx;

    // this cv is used by the worker search thread to wait until a valid depth is received
    std::condition_variable cv;

    // searchDepth represents the depth of search requested by the "go" command. the
    // worker thread will wait until it is set to a positive value
    int searchDepth = -1;

    // only set to true when the "quit" command is received
    bool terminateWorker = false;

    void searchWorker();

    int calculateTimeToMove(int totalTime, int increment);

   public:
    UciFrontend()
        : pos(std::string(STARTING_POSITION_FEN)), stopper(10000), engine(stopper) {}

    void run();
};
