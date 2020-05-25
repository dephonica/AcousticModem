#pragma once

#include <vector>

#include "Configuration.h" // for PCMTYPE

namespace ADSSS {
namespace PcmHelpers {

class IReferenceSource
{
public:
    virtual unsigned GetSampleRate() const = 0;

    virtual const std::vector<PCMTYPE>& GetReferenceData() const = 0;
    virtual std::vector<PCMTYPE> GetChunkData(size_t chunkIndex) const = 0;

    virtual std::vector<PCMTYPE> GetPrefixSamples() const = 0;
    virtual size_t GetPrefixSamplesCount() const = 0;

    virtual bool IsValid() const = 0;

    virtual size_t GetCapacityBytes() const = 0;
    virtual size_t GetReferenceChunksCount() const = 0;
    virtual size_t GetSamplesPerChunk() const = 0;

    virtual ~IReferenceSource() = default;
};

} // namespace PcmHelpers
} // namespace ADSSS
