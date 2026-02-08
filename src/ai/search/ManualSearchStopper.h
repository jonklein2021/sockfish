#include "src/ai/search/SearchStopper.h"

#include <atomic>

// Only aborts search when overrideAndAbort is called
// Used by UCI loop only
class ManualSearchStopper : public SearchStopper {
   private:
    std::atomic<bool> stopped;

   public:
    void reset() override;

    void overrideAndAbort() override;

    bool isStopped() override;
};
