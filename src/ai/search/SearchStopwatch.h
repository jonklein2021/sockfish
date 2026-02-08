#include "src/ai/search/SearchStopper.h"

#include <atomic>
#include <chrono>

class SearchStopwatch : public SearchStopper {
   private:
    std::chrono::steady_clock::time_point startTime;
    std::chrono::milliseconds timeLimit;

    std::atomic<bool> stopped;

   public:
    SearchStopwatch(int timeLimitMs);

    void reset() override;

    void overrideAndAbort() override;

    bool isStopped() override;
};
