#include "FirBlockConvolver.h"

#include "KernelConverter.h"

namespace ADSSS {
namespace FIR {

FirBlockConvolver::FirBlockConvolver(const FirKernelSource& kernelSource, size_t chunkSize)
    : taps_(kernelSource.GetTaps())
    , fftSize_(KernelConverter::GetDesiredSizeOfFft(taps_, chunkSize))
    , chunkSize_(chunkSize)
    , fftEngine_(fftSize_)
    , storingBuffer_(fftSize_)
    , processingBuffer_(fftSize_)
{
    Flush();

    auto kernelImpulse = KernelConverter::ComplexKernelToImpulseResponse(kernelSource.ToComplexKernel());

    std::vector<PCMTYPE> paddedImpulse(fftSize_);
    std::copy(kernelImpulse.begin(), kernelImpulse.end(), paddedImpulse.begin());

    auto complexKernel = KernelConverter::ImpulseResponseToComplexKernel(paddedImpulse);

    fftEngine_.SetConvolutionKernel(complexKernel);
}

void FirBlockConvolver::Flush()
{
    std::fill(storingBuffer_.begin(), storingBuffer_.end(), 0);
    std::fill(processingBuffer_.begin(), processingBuffer_.end(), 0);
}

size_t FirBlockConvolver::Convolve(const std::vector<PCMTYPE>& inputBuffer, std::vector<PCMTYPE>& outputBuffer)
{
    std::copy(inputBuffer.begin(), inputBuffer.begin() + chunkSize_, processingBuffer_.begin());

    fftEngine_.ExecuteConvolution(processingBuffer_, storingBuffer_);

    if (ADSSS::Config::IsDebug && (fftSize_ / 2 + chunkSize_ / 2 + outputBuffer.size()) > storingBuffer_.size()) {
        std::cerr << "WARNING!!! Buffer overflow in FirBlockConvolver. Review configuration values ExpectedReferenceSamplerateHz, "
                  << "FIRTaps, DataChunkMilliseconds." << std::endl;
    }

    std::copy(storingBuffer_.begin() + taps_ / 2,
              storingBuffer_.begin() + taps_ / 2 + chunkSize_,
              outputBuffer.begin());

    return chunkSize_;
}

} // namespace FIR
} // namespace ADSSS
