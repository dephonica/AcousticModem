#pragma once

#include <iostream>
#include <vector>

#include "Configuration.h"

#include "EnvelopePoint.h"
#include "FftEngineHelper.h"
#include "WindowFunctions.h"

namespace ADSSS {
namespace Encoding {

class EnvelopeGenerator
{
private:
    unsigned sampleRate_;
    float hertzPerPoint_;

    ADSSS::FFT::WindowFunctions windowEngine_;

    std::vector<ADSSS::FIR::EnvelopePoint> envelopePoints_;

    void ApplyGain(size_t startIndex, size_t endIndex, float gainDb);

public:
    EnvelopeGenerator(unsigned sampleRate, unsigned taps);
    void PushMessage(const std::vector<uint8_t>& messageBits, const ADSSS::FFT::FftEngineHelper& sourceChunk);
    void SetChunkGain(size_t chunkIndex, float gainDb);

    unsigned GetSampleRate() const { return sampleRate_; }

    const std::vector<ADSSS::FIR::EnvelopePoint>& GetEnvelopePoints() const { return envelopePoints_; }
};

} // namespace Encoding
} // namespace ADSSS
