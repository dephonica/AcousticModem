#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "Configuration.h"

namespace ADSSS {
namespace PcmHelpers {

enum class NormalizationTypes
{
    Amplitude,
    Histogramm
};

class PcmNormalization
{
private:
    static PCMTYPE GetAverageAmplitude(const std::vector<PCMTYPE>& buffer, PCMTYPE peakThreshold);

public:
    static void Normalize(std::vector<PCMTYPE>& buffer, PCMTYPE toAmplitude, NormalizationTypes normalizationType);
};

} // namespace PcmHelpers
} // namespace ADSSS
