#include <limits>
#include <stdint.h>

// linear congruential generator based on https://rosettacode.org/wiki/Linear_congruential_generator
class PRNG {
   private:
    int _a, _c;
    uint32_t _seed;

    static constexpr uint32_t generateSeed() {
        const char *timeStr = __TIME__;
        return static_cast<uint32_t>(timeStr[0] * 3600 + timeStr[1] * 360 + timeStr[3] * 60 +
                                     timeStr[4] * 10 + timeStr[6] + timeStr[7]);
    }

   public:
    // Microsoft LCG formula
    constexpr PRNG(uint32_t seed = generateSeed())
        : _a(214013), _c(2531011), _seed(seed) {}

    // returns the next pseudo random number in the sequence
    constexpr uint64_t next() {
        return _seed = (_a * _seed + _c) & std::numeric_limits<uint64_t>::max();
    }
};
