#pragma once

#include <complex>
#include <vector>

#include "Configuration.h"

#include "EnvelopePoint.h"

namespace ADSSS {
namespace FIR {

class KernelConverter
{
public:
    static std::vector<std::complex<PCMTYPE>> EnvelopeToComplexKernel(const std::vector<EnvelopePoint>& envelope);
    static std::vector<EnvelopePoint> ComplexKernelToEnvelope(const std::vector<std::complex<PCMTYPE>>& complexKernel);
    static std::vector<PCMTYPE> ComplexKernelToImpulseResponse(const std::vector<std::complex<PCMTYPE>>& complexKernel);
    static std::vector<std::complex<PCMTYPE>> ImpulseResponseToComplexKernel(const std::vector<PCMTYPE>& impulseResponse);

    static std::vector<std::complex<PCMTYPE>> GetReferenceKernel(size_t size);

    static std::vector<PCMTYPE> ResizeDoubles(const std::vector<PCMTYPE>& source, size_t targetPointsCount);
    static std::vector<std::complex<PCMTYPE>> ResizeComplexKernel(const std::vector<std::complex<PCMTYPE>>& sourceKernel,
                                                                  size_t targetSize);

    static size_t GetDesiredSizeOfFft(size_t taps, size_t chunkSize);
};

} // namespace FIR
} // namespace ADSSS
