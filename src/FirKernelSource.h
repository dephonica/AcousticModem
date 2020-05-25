#pragma once

#include <complex>
#include <vector>

#include "Configuration.h"

#include "EnvelopePoint.h"

namespace ADSSS {
namespace FIR {

class FirKernelSource
{
private:
    unsigned sampleRate_;

    std::vector<EnvelopePoint> points_;

public:
    FirKernelSource();
    FirKernelSource(unsigned sampleRate, const std::vector<EnvelopePoint>& points);

    bool Equals(const FirKernelSource& toSource) const;
    void SetFrom(const FirKernelSource& source);

    FirKernelSource GetAdjusted(unsigned targetSampleRate, size_t targetTaps, float gainDecayValue = 0.7f) const;

    std::vector<std::complex<PCMTYPE>> ToComplexKernel() const;
    std::vector<std::complex<PCMTYPE>> ToComplexKernel(unsigned sampleRate, size_t taps, float gainDecayValue = 0.7f) const;

    size_t GetTaps() const { return points_.size() > 0 ? (points_.size() - 1) * 2 : 0; }

    const std::vector<EnvelopePoint> GetPoints() const { return points_; }

    unsigned GetSampleRate() const { return sampleRate_; }
};

} // namespace FIR
} // namespace ADSSS
