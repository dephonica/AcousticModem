#pragma once

#include <iostream>
#include <vector>

namespace ADSSS {
namespace Bitwise {

class BitBuffer
{
private:
    static const bool IsMsbFirstOrder = true;
    std::vector<uint8_t> bufferBits_;

public:
    void InitWithBytes(const std::vector<uint8_t>& initBuffer);

    void Clear();
    void PushBit(uint8_t bitValue);
    void PushBytes(const std::vector<uint8_t>& bytesToPush);
    uint8_t PopBit();

    size_t GetBitsCount() const { return bufferBits_.size(); }

    bool IsEmpty() const { return bufferBits_.empty(); }

    size_t GetBytesCount() const { return (bufferBits_.size() + 7) / 8; }

    std::vector<uint8_t> PopBits(size_t bitsToPop);
    uint8_t PopByte();

    std::vector<uint8_t> GetBytes();
};

} // namespace Bitwise
} // namespace ADSSS
