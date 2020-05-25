#pragma once

#include <cmath>

#include <limits>

namespace ADSSS {
namespace Math {

class LogConversions
{
public:
    static double ValueToDecibels(double value) { return 20 * std::log10(value + std::numeric_limits<double>::epsilon()); }
    static double DecibelsToValue(double db) { return std::pow(10.0, db / 20); }

    static float ValueToDecibels(float value) { return 20 * std::log10(value + std::numeric_limits<float>::epsilon()); }
    static float DecibelsToValue(float db) { return std::pow(10.f, db / 20); }
};

} // namespace Math
} // namespace ADSSS
