#include "ReferenceSource.h"
#include "PcmNormalization.h"

#include <algorithm>
#include <iostream>

namespace ADSSS {
namespace PcmHelpers {

ReferenceSource::ReferenceSource()
    : sampleRate_(ADSSS::Config::ExpectedReferenceSamplerateHz)
    , startChunkOffset_(ADSSS::Config::StartChunkMarginMilliseconds * ADSSS::Config::ExpectedReferenceSamplerateHz / 1000)
    , isValid_(false)
{
}

ReferenceSource::ReferenceSource(unsigned sampleRate, const PCMTYPE* pcmData, size_t pcmSamples)
    : sampleRate_(sampleRate)
    , startChunkOffset_(ADSSS::Config::StartChunkMarginMilliseconds * ADSSS::Config::ExpectedReferenceSamplerateHz / 1000)
    , referenceData_(pcmData, pcmData + pcmSamples)
    , isValid_(false)
{
    NormalizeData();
    PreprocessPcm();
}

ReferenceSource::ReferenceSource(unsigned sampleRate, std::vector<PCMTYPE>&& pcmData)
    : sampleRate_(sampleRate)
    , startChunkOffset_(ADSSS::Config::StartChunkMarginMilliseconds * ADSSS::Config::ExpectedReferenceSamplerateHz / 1000)
    , referenceData_(std::move(pcmData))
    , isValid_(false)
{
    NormalizeData();
    PreprocessPcm();
}

ReferenceSource::ReferenceSource(unsigned sampleRate, const std::vector<PCMTYPE>& pcmData, size_t pcmSamples)
    : sampleRate_(sampleRate)
    , startChunkOffset_(ADSSS::Config::StartChunkMarginMilliseconds * ADSSS::Config::ExpectedReferenceSamplerateHz / 1000)
    , referenceData_(pcmData.begin(), pcmData.begin() + pcmSamples)
    , isValid_(false)
{
    NormalizeData();
    PreprocessPcm();
}

ReferenceSource::ReferenceSource(unsigned sampleRate, const std::vector<PCMTYPE>& pcmData)
    : sampleRate_(sampleRate)
    , startChunkOffset_(ADSSS::Config::StartChunkMarginMilliseconds * ADSSS::Config::ExpectedReferenceSamplerateHz / 1000)
    , referenceData_(pcmData.begin(), pcmData.end())
    , isValid_(false)
{
    NormalizeData();
    PreprocessPcm();
}

ReferenceSource::~ReferenceSource()
{
}

void ReferenceSource::NormalizeData()
{
    if (sampleRate_ == ADSSS::Config::ExpectedReferenceSamplerateHz) {
        isValid_ = true;

        PcmNormalization::Normalize(referenceData_, ADSSS::Config::ReferenceNormalizationAmplitude, NormalizationTypes::Histogramm);
    }
}

void ReferenceSource::PreprocessPcm()
{
    samplesPerDataChunk_ = static_cast<unsigned>(sampleRate_ * ADSSS::Config::DataChunkMilliseconds / 1000.0);
}

std::vector<PCMTYPE> ReferenceSource::GetChunkData(size_t chunkIndex) const
{
    size_t singleChunkSamples = GetSamplesPerChunk();

    std::vector<PCMTYPE> resultData(singleChunkSamples);
    size_t referenceOffset = startChunkOffset_ + singleChunkSamples * chunkIndex;
    size_t samplesToCopy = std::min(referenceData_.size() - referenceOffset, singleChunkSamples);

    std::copy(referenceData_.begin() + referenceOffset, referenceData_.begin() + referenceOffset + samplesToCopy, resultData.begin());

    return resultData;
}

std::vector<PCMTYPE> ReferenceSource::GetPrefixSamples() const
{
    std::vector<PCMTYPE> resultData(startChunkOffset_);
    std::copy(referenceData_.begin(), referenceData_.begin() + startChunkOffset_, resultData.begin());

    return resultData;
}

} // namespace PcmHelpers
} // namespace ADSSS
