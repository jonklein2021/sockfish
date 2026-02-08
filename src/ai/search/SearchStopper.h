#pragma once

// Base interface that tells Searcher when to stop running
class SearchStopper {
   public:
    virtual ~SearchStopper() = default;

    virtual void reset() = 0;

    virtual void overrideAndAbort() = 0;

    virtual bool isStopped() = 0;
};
