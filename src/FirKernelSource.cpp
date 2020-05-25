#include "FirKernelSource.h"
#include "KernelConverter.h"

#include <iostream>

namespace ADSSS {
namespace FIR {

FirKernelSource::FirKernelSource()
{
}

FirKernelSource::FirKernelSource(unsigned sampleRate, const std::vector<EnvelopePoint>& points)
{
    sampleRate_ = sampleRate;
    points_ = points;
}

bool FirKernelSource::Equals(const FirKernelSource& toSource) const
{
    if (toSource.GetTaps() != GetTaps()) {
        return false;
    }

    return toSource.GetPoints() == GetPoints();
}

void FirKernelSource::SetFrom(const FirKernelSource& source)
{
    sampleRate_ = source.GetSampleRate();
    points_ = source.GetPoints();
}

FirKernelSource FirKernelSource::GetAdjusted(unsigned targetSampleRate, size_t targetTaps, float gainDecayValue) const
{
    if (ADSSS::Config::IsDebug && targetTaps % 2 != 0) {
        std::cerr << "Can't adjust FIR envelope kernel for provided odd Taps number" << std::endl;
        return FirKernelSource();
    }

    if (ADSSS::Config::IsDebug && (sampleRate_ < 8000 || targetSampleRate < 8000)) {
        std::cerr << "Can't adjust FIR envelope kernel - invalid source or target sample rates" << std::endl;
        return FirKernelSource();
    }

    auto targetPoints = targetTaps / 2 + 1;

    size_t resampledPointsCount = targetPoints * sampleRate_ / targetSampleRate;

    std::vector<PCMTYPE> pointsGain(points_.size()), pointsPhase(points_.size());

    for (size_t n = 0; n < points_.size(); n++) {
        pointsGain[n] = points_[n].Gain;
        pointsPhase[n] = points_[n].Phase;
    }

    auto resultGains = KernelConverter::ResizeDoubles(pointsGain, resampledPointsCount);
    auto resultPhases = KernelConverter::ResizeDoubles(pointsPhase, resampledPointsCount);

    std::vector<EnvelopePoint> result(targetPoints);

    float lastPointGain = 0, lastPointPhase = 0;

    for (size_t n = 0; n < targetPoints; n++) {
        if (n < resampledPointsCount) {
            lastPointGain = resultGains[n];
            lastPointPhase = resultPhases[n];
        }
        else {
            lastPointGain *= gainDecayValue;
        }

        result[n].Gain = lastPointGain;
        result[n].Phase = lastPointPhase;
    }

    return FirKernelSource(targetSampleRate, result);
}

std::vector<std::complex<PCMTYPE>> FirKernelSource::ToComplexKernel() const
{
    return KernelConverter::EnvelopeToComplexKernel(GetPoints());
}

std::vector<std::complex<PCMTYPE>> FirKernelSource::ToComplexKernel(unsigned sampleRate, size_t taps, float gainDecayValue) const
{
    return KernelConverter::EnvelopeToComplexKernel(GetAdjusted(sampleRate, taps, gainDecayValue).GetPoints());
}

} // namespace FIR
} // namespace ADSSS
