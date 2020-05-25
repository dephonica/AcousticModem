#include "RandomGenerator.h"

namespace ADSSS {

RandomGenerator::RandomGenerator(const std::vector<uint8_t>& initSeed)
{
    initSeed_ = initSeed;

    Reset();
}

void RandomGenerator::Reset()
{
    seed_ = initSeed_;

    for (size_t i = 0; i < initSeed_.size(); i++) {
        SwapSeed();
    }
}

void RandomGenerator::SwapSeed()
{
    auto& seed = seed_;

    if ((seed[0] & (1 << 0)) != 0)
        seed[7] ^= (1 << 3);
    if ((seed[8] & (1 << 0)) != 0)
        seed[15] ^= (1 << 0);
    if ((seed[8] & (1 << 1)) != 0)
        seed[0] ^= (1 << 0);
    if ((seed[5] & (1 << 1)) != 0)
        seed[2] ^= (1 << 5);
    if ((seed[1] & (1 << 6)) != 0)
        seed[5] ^= (1 << 4);
    if ((seed[8] & (1 << 5)) != 0)
        seed[3] ^= (1 << 5);
    if ((seed[1] & (1 << 5)) != 0)
        seed[4] ^= (1 << 4);
    if ((seed[10] & (1 << 2)) != 0)
        seed[11] ^= (1 << 2);
    if ((seed[10] & (1 << 1)) != 0)
        seed[4] ^= (1 << 3);
    if ((seed[5] & (1 << 1)) != 0)
        seed[13] ^= (1 << 1);
    if ((seed[9] & (1 << 3)) != 0)
        seed[11] ^= (1 << 1);
    if ((seed[0] & (1 << 1)) != 0)
        seed[4] ^= (1 << 3);
    if ((seed[7] & (1 << 0)) != 0)
        seed[10] ^= (1 << 1);
    if ((seed[11] & (1 << 0)) != 0)
        seed[4] ^= (1 << 1);
    if ((seed[15] & (1 << 5)) != 0)
        seed[5] ^= (1 << 5);
    if ((seed[12] & (1 << 2)) != 0)
        seed[7] ^= (1 << 0);
    if ((seed[8] & (1 << 4)) != 0)
        seed[5] ^= (1 << 0);
    if ((seed[13] & (1 << 4)) != 0)
        seed[7] ^= (1 << 5);
    if ((seed[0] & (1 << 0)) != 0)
        seed[0] ^= (1 << 0);
    if ((seed[1] & (1 << 6)) != 0)
        seed[11] ^= (1 << 5);
    if ((seed[6] & (1 << 5)) != 0)
        seed[3] ^= (1 << 3);
    if ((seed[11] & (1 << 3)) != 0)
        seed[10] ^= (1 << 3);
    if ((seed[0] & (1 << 0)) != 0)
        seed[7] ^= (1 << 4);
    if ((seed[13] & (1 << 6)) != 0)
        seed[7] ^= (1 << 2);
    if ((seed[15] & (1 << 4)) != 0)
        seed[11] ^= (1 << 2);
    if ((seed[14] & (1 << 1)) != 0)
        seed[9] ^= (1 << 5);
    if ((seed[10] & (1 << 5)) != 0)
        seed[3] ^= (1 << 4);
    if ((seed[1] & (1 << 1)) != 0)
        seed[0] ^= (1 << 6);
    if ((seed[0] & (1 << 3)) != 0)
        seed[6] ^= (1 << 6);
    if ((seed[1] & (1 << 6)) != 0)
        seed[6] ^= (1 << 0);
    if ((seed[0] & (1 << 5)) != 0)
        seed[1] ^= (1 << 5);
    if ((seed[3] & (1 << 6)) != 0)
        seed[15] ^= (1 << 0);
    if ((seed[10] & (1 << 5)) != 0)
        seed[2] ^= (1 << 3);
    if ((seed[4] & (1 << 2)) != 0)
        seed[15] ^= (1 << 1);
    if ((seed[3] & (1 << 2)) != 0)
        seed[13] ^= (1 << 6);
    if ((seed[4] & (1 << 0)) != 0)
        seed[7] ^= (1 << 5);
    if ((seed[1] & (1 << 3)) != 0)
        seed[9] ^= (1 << 2);
    if ((seed[9] & (1 << 3)) != 0)
        seed[3] ^= (1 << 6);
    if ((seed[3] & (1 << 4)) != 0)
        seed[5] ^= (1 << 0);
    if ((seed[2] & (1 << 6)) != 0)
        seed[9] ^= (1 << 5);
    if ((seed[13] & (1 << 1)) != 0)
        seed[15] ^= (1 << 2);
    if ((seed[0] & (1 << 3)) != 0)
        seed[10] ^= (1 << 4);
    if ((seed[8] & (1 << 1)) != 0)
        seed[11] ^= (1 << 5);
    if ((seed[1] & (1 << 3)) != 0)
        seed[12] ^= (1 << 6);
    if ((seed[4] & (1 << 1)) != 0)
        seed[9] ^= (1 << 4);
    if ((seed[5] & (1 << 4)) != 0)
        seed[7] ^= (1 << 0);
    if ((seed[8] & (1 << 1)) != 0)
        seed[4] ^= (1 << 4);
    if ((seed[15] & (1 << 5)) != 0)
        seed[0] ^= (1 << 2);
    if ((seed[12] & (1 << 0)) != 0)
        seed[0] ^= (1 << 0);
    if ((seed[1] & (1 << 1)) != 0)
        seed[12] ^= (1 << 3);
    if ((seed[13] & (1 << 6)) != 0)
        seed[12] ^= (1 << 2);
    if ((seed[13] & (1 << 3)) != 0)
        seed[13] ^= (1 << 4);
    if ((seed[14] & (1 << 2)) != 0)
        seed[13] ^= (1 << 6);
    if ((seed[4] & (1 << 6)) != 0)
        seed[6] ^= (1 << 1);
    if ((seed[11] & (1 << 6)) != 0)
        seed[12] ^= (1 << 6);
    if ((seed[3] & (1 << 0)) != 0)
        seed[10] ^= (1 << 5);
    if ((seed[3] & (1 << 2)) != 0)
        seed[15] ^= (1 << 6);
    if ((seed[7] & (1 << 4)) != 0)
        seed[13] ^= (1 << 0);
    if ((seed[7] & (1 << 0)) != 0)
        seed[12] ^= (1 << 3);
    if ((seed[13] & (1 << 0)) != 0)
        seed[4] ^= (1 << 0);
    if ((seed[4] & (1 << 4)) != 0)
        seed[12] ^= (1 << 4);
    if ((seed[0] & (1 << 0)) != 0)
        seed[1] ^= (1 << 5);
    if ((seed[11] & (1 << 5)) != 0)
        seed[6] ^= (1 << 1);
    if ((seed[6] & (1 << 6)) != 0)
        seed[5] ^= (1 << 5);

    auto firstBit = (seed[0] & 0x01) != 0;

    uint8_t value;

    for (size_t i = 0; i < 16 - 1; i++) {
        value = seed[i];
        auto nextValue = seed[i + 1];

        value ^= 0xae;

        auto currentBit = (nextValue & 0x01) != 0;
        value >>= 1;
        value &= 0xff - 0x80;

        if (currentBit) {
            value |= 0x80;
        }

        seed[i] = value;
    }

    value = seed[16 - 1];
    value >>= 1;
    value &= 0xff - 0x80;

    if (firstBit) {
        value |= 0x80;
    }

    seed[16 - 1] = value;
}

uint8_t RandomGenerator::GetNextByte()
{
    uint8_t resultByte = 0;

    for (size_t i = 0; i < seed_.size(); i++) {
        resultByte ^= seed_[i];
    }

    SwapSeed();

    return resultByte;
}

} // namespace ADSSS
