#pragma once

#include <string>
#include <vector>

#include "Configuration.h"
#include "IReferenceSource.h"

namespace ADSSS {
namespace PcmHelpers {

class ReferenceSource : public IReferenceSource
{
protected:
    unsigned sampleRate_;
    size_t samplesPerDataChunk_;
    size_t startChunkOffset_;

    std::vector<PCMTYPE> referenceData_;

    bool isValid_;

    void NormalizeData();
    void PreprocessPcm();

public:
    ReferenceSource();
    ReferenceSource(unsigned sampleRate, const PCMTYPE* pcmData, size_t pcmSamples);
    ReferenceSource(unsigned sampleRate, std::vector<PCMTYPE>&& pcmData);
    ReferenceSource(unsigned sampleRate, const std::vector<PCMTYPE>& pcmData);
    ReferenceSource(unsigned sampleRate, const std::vector<PCMTYPE>& pcmData, size_t pcmSamples);

    ~ReferenceSource() override;

    unsigned GetSampleRate() const override { return sampleRate_; }

    const std::vector<PCMTYPE>& GetReferenceData() const override { return referenceData_; }

    bool IsValid() const override { return isValid_; }

    size_t GetCapacityBytes() const override
    {
        return ((referenceData_.size() - startChunkOffset_) / samplesPerDataChunk_) * ADSSS::Config::BitsPerDataChunk / 8;
    }

    size_t GetReferenceChunksCount() const override { return (referenceData_.size() - startChunkOffset_ + samplesPerDataChunk_ - 1) / samplesPerDataChunk_; }

    size_t GetSamplesPerChunk() const override { return samplesPerDataChunk_; }

    std::vector<PCMTYPE> GetChunkData(size_t chunkIndex) const override;

    std::vector<PCMTYPE> GetPrefixSamples() const override;

    size_t GetPrefixSamplesCount() const override { return startChunkOffset_; }
};

} // namespace PcmHelpers
} // namespace ADSSS
