#include "BitBuffer.h"

namespace ADSSS {
namespace Bitwise {

void BitBuffer::InitWithBytes(const std::vector<uint8_t>& initBuffer)
{
    Clear();
    PushBytes(initBuffer);
}

void BitBuffer::Clear()
{
    bufferBits_.clear();
}

void BitBuffer::PushBit(uint8_t bitValue)
{
    bufferBits_.push_back(bitValue != 0);
}

void BitBuffer::PushBytes(const std::vector<uint8_t>& bytesToPush)
{
    size_t bitsToPush = bytesToPush.size() * 8;

    for (size_t n = 0; n < bitsToPush; n++) {
        auto byteIndex = n / 8;
        auto bitIndex = n % 8;

        if (IsMsbFirstOrder) {
            bitIndex = 7 - bitIndex;
        }

        uint8_t bitValue = (bytesToPush[byteIndex] & (1 << bitIndex)) != 0 ? 1 : 0;

        PushBit(bitValue);
    }
}

uint8_t BitBuffer::PopBit()
{
    if (bufferBits_.size() == 0) {
        return 0;
    }

    auto bitValue = bufferBits_[0];
    bufferBits_.erase(bufferBits_.begin());

    return bitValue;
}

std::vector<uint8_t> BitBuffer::PopBits(size_t bitsToPop)
{
    std::vector<uint8_t> bitResult(bitsToPop);

    for (size_t n = 0; n < bitsToPop; n++) {
        if (IsEmpty()) {
            break;
        }

        bitResult[n] = PopBit();
    }

    return bitResult;
}

uint8_t BitBuffer::PopByte()
{
    uint8_t resultByte = 0;

    for (unsigned n = 0; n < 8; n++) {
        if (IsEmpty()) {
            break;
        }

        auto bitIndex = n;

        if (IsMsbFirstOrder) {
            bitIndex = 7 - bitIndex;
        }

        auto bitValue = PopBit();

        if (bitValue != 0) {
            resultByte |= uint8_t(1 << bitIndex);
        }
    }

    return resultByte;
}

std::vector<uint8_t> BitBuffer::GetBytes()
{
    std::vector<uint8_t> resultBytes(GetBytesCount());

    for (size_t n = 0; n < resultBytes.size(); n++) {
        resultBytes[n] = PopByte();
    }

    return resultBytes;
}

} // namespace Bitwise
} // namespace ADSSS
