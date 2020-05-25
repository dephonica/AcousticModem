#pragma once

#include <iostream>
#include <vector>

#include "Configuration.h"

#include "FftEngine.h"
#include "WindowFunctions.h"

namespace ADSSS {
namespace FFT {

class FftEngineHelper
{
private:
    WindowFunctionTypes windowFunction_;
    WindowFunctions windowEngine_;

    unsigned sampleRate_;

    FftEngine fftEngine_;

    std::vector<PCMTYPE> realBuffer_;
    std::vector<std::complex<PCMTYPE>> complexBuffer_;

    std::vector<float> responseCorrection_;

    double complexResultScale_;

    bool isCorrected_;

public:
    FftEngineHelper(unsigned sampleRate, size_t fftSize, WindowFunctionTypes windowFunction);

    void ApplyResponseCorrection(const std::vector<float>& encodedSource, const std::vector<float>& referenceSource);

    std::vector<float> GetResponse(const std::vector<PCMTYPE>& chunkToProcess, float filterResponse = 0.005);
    void PushChunk(const std::vector<float>& chunkToProcess);

    double GetChunkLevel(size_t chunkIndex) const;
    double GetChunkLevel(float startFrequency, float bandWidth) const;

    std::vector<float> GetChunkProfile(size_t chunkIndex) const;
    std::vector<float> GetChunkProfile(float startFrequency, float bandWidth) const;
};

} // namespace FFT
} // namespace ADSSS
