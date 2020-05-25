#include "FftEngineHelper.h"

#include <algorithm>
#include "LogConversions.h"

namespace ADSSS {
namespace FFT {

FftEngineHelper::FftEngineHelper(unsigned sampleRate, size_t fftSize, WindowFunctionTypes windowFunction)
    : windowFunction_(windowFunction)
    , windowEngine_(windowFunction, 1)
    , sampleRate_(sampleRate)
    , fftEngine_(fftSize)
    , realBuffer_(fftEngine_.GetFFTSize())
    , complexBuffer_(fftEngine_.GetComplexSize())
    , responseCorrection_(fftEngine_.GetComplexSize())
    , isCorrected_(false)
{
    std::fill(responseCorrection_.begin(), responseCorrection_.end(), 1.0);
}

void FftEngineHelper::ApplyResponseCorrection(const std::vector<float>& encodedSource, const std::vector<float>& referenceSource)
{
    auto encodedResponse = GetResponse(encodedSource);
    auto referenceResponse = GetResponse(referenceSource);

    for (size_t n = 0; n < responseCorrection_.size(); n++)
    {
        if (encodedResponse[n] != 0)
        {
            responseCorrection_[n] = referenceResponse[n] / encodedResponse[n];
        } else
        {
            responseCorrection_[n] = 1.0;
        }
    }

    isCorrected_ = true;
}

std::vector<float> FftEngineHelper::GetResponse(const std::vector<PCMTYPE>& chunkToProcess, float filterResponse)
{
    const size_t blockSize = fftEngine_.GetFFTSize() / 2;
    size_t blocksCount = 1 + chunkToProcess.size() / blockSize;

    std::vector<PCMTYPE> blockBuffer(blockSize);
    std::vector<PCMTYPE> resultResponse(fftEngine_.GetComplexSize());

    size_t blockPointer = 0;

    for (size_t blockIndex = 0; blockIndex < blocksCount; blockIndex++)
    {
        size_t samplesToCopy = std::min(chunkToProcess.size() - blockPointer, blockSize);

        std::copy(chunkToProcess.begin() + blockPointer,
                chunkToProcess.begin() + blockPointer + samplesToCopy,
                blockBuffer.begin());
        
        PushChunk(blockBuffer);

        float scale = float(blockSize) / samplesToCopy;

        for (size_t n = 0; n < resultResponse.size(); n++)
        {
            resultResponse[n] += static_cast<double>(std::abs(complexBuffer_[n])) * scale;
        }
    }

    double scale = complexResultScale_ / blocksCount / realBuffer_.size();

    for (size_t n = 0; n < resultResponse.size(); n++)
    {
        resultResponse[n] *= scale;
    }

    double filterValue = 0.0;

    for (size_t n = 0; n < resultResponse.size(); n++)
    {
        filterValue = filterValue * (1.0 - filterResponse) + resultResponse[n] * filterResponse;
        resultResponse[n] = filterValue;
    }

    return resultResponse;
}

void FftEngineHelper::PushChunk(const std::vector<PCMTYPE>& chunkToProcess)
{
    if (windowEngine_.GetWindowSize() != chunkToProcess.size()) {
        windowEngine_.Reset(windowFunction_, chunkToProcess.size());
    }

    std::fill(realBuffer_.begin(), realBuffer_.end(), 0);
    std::copy(chunkToProcess.begin(), chunkToProcess.end(), realBuffer_.begin());
    windowEngine_.Apply(realBuffer_);

    fftEngine_.ExecuteR2C(realBuffer_, complexBuffer_);

    complexResultScale_ = realBuffer_.size() / chunkToProcess.size() * windowEngine_.GetScale();
}

double FftEngineHelper::GetChunkLevel(size_t chunkIndex) const
{
    float chunkStartFreq = ADSSS::Config::LowerUsableFrequencyHz + chunkIndex * ADSSS::Config::BitFullBandwidthHz();
    float bitStartFreq = chunkStartFreq + ADSSS::Config::BitDecodeMarginHz;

    return GetChunkLevel(bitStartFreq, ADSSS::Config::BitFullBandwidthHz() - ADSSS::Config::BitDecodeMarginHz * 2);
}

double FftEngineHelper::GetChunkLevel(float startFrequency, float bandWidth) const
{
    float fftBandwidth = sampleRate_ / 2;
    float complexItems = complexBuffer_.size() - 1;
    float hertzPerItem = fftBandwidth / complexItems;

    size_t startItem = static_cast<size_t>(startFrequency / hertzPerItem);
    size_t endItem = std::min(startItem + static_cast<size_t>(bandWidth / hertzPerItem), static_cast<size_t>(complexItems - 1));

    double powerAccumulator = 0;

    if (isCorrected_)
    {
        for (size_t itemIndex = startItem; itemIndex <= endItem; itemIndex++) {
            powerAccumulator += static_cast<double>(std::abs(complexBuffer_[itemIndex])) * responseCorrection_[itemIndex];
        }
    } else
    {
        for (size_t itemIndex = startItem; itemIndex <= endItem; itemIndex++) {
            powerAccumulator += static_cast<double>(std::abs(complexBuffer_[itemIndex]));
        }
    }

    auto itemsAccumulated = endItem - startItem;
    powerAccumulator /= itemsAccumulated;
    powerAccumulator /= realBuffer_.size();
    powerAccumulator *= complexResultScale_;

    return powerAccumulator;
}

std::vector<float> FftEngineHelper::GetChunkProfile(size_t chunkIndex) const
{
    float chunkStartFreq = ADSSS::Config::LowerUsableFrequencyHz + chunkIndex * ADSSS::Config::BitFullBandwidthHz();
    float bitStartFreq = chunkStartFreq + ADSSS::Config::BitDecodeMarginHz;

    return GetChunkProfile(bitStartFreq, ADSSS::Config::BitFullBandwidthHz() - ADSSS::Config::BitDecodeMarginHz * 2);
}

std::vector<float> FftEngineHelper::GetChunkProfile(float startFrequency, float bandWidth) const
{
    float fftBandwidth = sampleRate_ / 2;
    float complexItems = complexBuffer_.size() - 1;
    float hertzPerItem = fftBandwidth / complexItems;

    size_t startItem = static_cast<size_t>(startFrequency / hertzPerItem);
    size_t endItem = std::min(startItem + static_cast<size_t>(bandWidth / hertzPerItem), static_cast<size_t>(complexItems - 1));

    std::vector<float> resultVector;

    double scale = complexResultScale_ / realBuffer_.size();

    if (isCorrected_)
    {
        for (size_t itemIndex = startItem; itemIndex <= endItem; itemIndex++) {
            resultVector.push_back(static_cast<float>(std::abs(complexBuffer_[itemIndex])) * responseCorrection_[itemIndex] * scale);
        }
    } else
    {
        for (size_t itemIndex = startItem; itemIndex <= endItem; itemIndex++) {
            resultVector.push_back(static_cast<float>(std::abs(complexBuffer_[itemIndex]) * scale));
        }
    }

    return resultVector;
}

} // namespace FFT
} // namespace ADSSS
