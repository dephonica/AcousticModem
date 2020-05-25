#pragma once

#include <iostream>
#include <vector>

#include "Configuration.h"

#include "BitBuffer.h"
#include "EnvelopeGenerator.h"
#include "FftEngineHelper.h"
#include "IReferenceSource.h"
#include "RandomGenerator.h"

namespace ADSSS {

class Encoder
{
private:
    const PcmHelpers::IReferenceSource& referenceSource_;

    FFT::FftEngineHelper fftHelper_;
    Encoding::EnvelopeGenerator envelopeGenerator_;

    std::vector<uint8_t> RandomizePayload(const std::vector<uint8_t>& payloadBytes);
    std::vector<PCMTYPE> EncodeData(const std::vector<uint8_t>& payloadBytes);

    void ApplyDataToChunk(std::vector<float>& chunkData, Bitwise::BitBuffer& bitBuffer);
    void PutChunkData(std::vector<float>& resultData, size_t chunkIndex, const std::vector<PCMTYPE>& chunkData);

public:
    explicit Encoder(const PcmHelpers::IReferenceSource& referenceSource);

    std::vector<PCMTYPE> Go(const std::vector<uint8_t>& payloadBytes);
};

} // namespace ADSSS
