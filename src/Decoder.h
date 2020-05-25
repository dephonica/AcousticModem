#pragma once

#include <iostream>
#include <vector>

#include "Configuration.h"

#include "BitBuffer.h"
#include "FftEngineHelper.h"
#include "IReferenceSource.h"
#include "RandomGenerator.h"

namespace ADSSS {

class Decoder
{
private:
    const PcmHelpers::IReferenceSource& encodedSource_;
    const PcmHelpers::IReferenceSource& referenceSource_;

    std::vector<uint8_t> DecodeChunk(ADSSS::FFT::FftEngineHelper& fftHelperCaptured);
    std::vector<double> MeasureChunkData(ADSSS::FFT::FftEngineHelper& fftHelperCaptured);

    void DerandomizePayload(std::vector<uint8_t>& payloadBytes);

public:
    explicit Decoder(const PcmHelpers::IReferenceSource& encodedSource, const PcmHelpers::IReferenceSource& referenceSource);

    std::vector<uint8_t> Go();
};

} // namespace ADSSS
