#pragma once

#include <vector>

#include "Configuration.h"

#include "FftEngineHelper.h"
#include "IReferenceSource.h"

namespace ADSSS {

class StreamDetector
{
private:
    bool isStreamDetected_;
    const unsigned sampleRate_;
    const int higherUsableFrequencyHz_;

    ADSSS::FFT::FftEngineHelper fftHelper_;

    float detectorFilterValue_;
    float beforeDetectorValue_, afterDetectorValue_;
    float detectorBeforeThresholdDb_, detectorAfterThresholdDb_;

    size_t samplesCollected_;
    std::vector<PCMTYPE> collectBuffer_;

    void PushChunk(const std::vector<PCMTYPE>& samplesData);

public:
    StreamDetector(unsigned sampleRate, int fftSize, float detectorFilterValue, float detectorBeforeThresholdDb,
        float detectorAfterThresholdDb);

    void PushSamples(const std::vector<PCMTYPE>& samplesData, size_t offset, size_t length);

    bool IsDetected() const
    {
        return beforeDetectorValue_ >= detectorBeforeThresholdDb_ &&
                afterDetectorValue_ >= detectorAfterThresholdDb_;
    }
};

} // namespace ADSSS