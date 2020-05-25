#pragma once

#include <vector>

#include "Configuration.h"

#include "FftEngine.h"
#include "FirKernelSource.h"

namespace ADSSS {
namespace FIR {

class FirBlockConvolver
{
private:
    size_t taps_, fftSize_, chunkSize_;

    ADSSS::FFT::FftEngine fftEngine_;

    std::vector<PCMTYPE> storingBuffer_, processingBuffer_;

public:
    explicit FirBlockConvolver(const ADSSS::FIR::FirKernelSource& kernelSource, size_t chunkSize);

    void Flush();
    size_t Convolve(const std::vector<PCMTYPE>& inputBuffer, std::vector<PCMTYPE>& outputBuffer);
};
} // namespace FIR
} // namespace ADSSS
