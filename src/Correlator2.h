#pragma once

#include "Configuration.h"
#include "FftEngine.h"
#include "IReferenceSource.h"
#include "WindowFunctions.h"

namespace ADSSS {

struct CorrelationResult
{
    size_t offset_ = 0;
    float correlation_ = -99999;
    size_t sampleRate_ = Config::ExpectedReferenceSamplerateHz;

    CorrelationResult() = default;

    CorrelationResult(size_t offset, float correlation)
        : offset_(offset)
        , correlation_(correlation)
    {
    }
};

class Correlator2
{
private:
    const size_t sampleRate_ = 0;
    const size_t referenceSamples_ = 0;

    ADSSS::FFT::FftEngine fftEngine_;

    std::vector<PCMTYPE> processingBuffer_;
    std::vector<std::complex<PCMTYPE>> complexBuffer_;

    static size_t GetFFTSize(size_t referenceLength)
    {
        size_t fftSize = 2;

        while (fftSize <= referenceLength) {
            fftSize *= 2;
        }

        return fftSize;
    }

    void InitializeReference(const ADSSS::PcmHelpers::IReferenceSource& referenceData);
    CorrelationResult FindOffsets(const std::vector<PCMTYPE>& samplesData);

public:
    Correlator2(const ADSSS::PcmHelpers::IReferenceSource& referenceData, size_t referenceLengthMs);
    CorrelationResult Calculate(const std::vector<PCMTYPE>& samplesData, size_t offset = 0);

    PCMTYPE GetCorrelationAtOffset(int offset)
    {
        return processingBuffer_[offset + fftEngine_.GetFFTSize() / 2];
    }
};

} // namespace ADSSS
