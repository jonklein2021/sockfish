#include "src/ai/search/SearchStopper.h"

#include <atomic>

// Aborts search only when overrideAndAbort is called
// Used for testing only
class ManualSearchStopper : public SearchStopper {
   private:
    std::atomic<bool> stopped;

   public:
    void reset() override;

    void overrideAndAbort() override;

    bool isStopped() override;
};
