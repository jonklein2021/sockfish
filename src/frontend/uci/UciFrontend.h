#include "src/ai/Engine.h"
#include "src/core/Position.h"
#include "src/core/types.h"

#include <atomic>
#include <thread>

class UciFrontend {
   private:
    Position pos;
    Engine engine;

    std::thread inputThread;
    std::atomic<bool> stopFlag = false;
    std::atomic<bool> running = true;

    void run();

   public:
    UciFrontend()
        : pos(std::string(STARTING_POSITION_FEN)) {}

    void start();

    void stop();

    bool stopRequested() const;
};
