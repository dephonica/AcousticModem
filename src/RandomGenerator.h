#pragma once

#include <iostream>
#include <vector>

namespace ADSSS {

class RandomGenerator
{
private:
    std::vector<uint8_t> initSeed_, seed_;

    void SwapSeed();

public:
    explicit RandomGenerator(const std::vector<uint8_t>& initSeed);

    void Reset();
    uint8_t GetNextByte();
};

} // namespace ADSSS
