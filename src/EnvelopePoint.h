#pragma once

#include <algorithm>
#include <cmath>

namespace ADSSS {
namespace FIR {

struct EnvelopePoint
{
    float Frequency;
    float Gain;
    float Phase;

    bool operator==(const EnvelopePoint& toPoint) const
    {
        return std::abs(Frequency - toPoint.Frequency) < 0.001f && std::abs(Gain - toPoint.Gain) < 0.001f &&
               std::abs(Phase - toPoint.Phase) < 0.001f;
    }
};

} // namespace FIR
} // namespace ADSSS
