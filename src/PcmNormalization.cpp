#include "PcmNormalization.h"

namespace ADSSS {
namespace PcmHelpers {

PCMTYPE PcmNormalization::GetAverageAmplitude(const std::vector<PCMTYPE>& buffer, PCMTYPE peakThreshold)
{
    size_t amplitudeHistogramm[256]{ 0 };

    for (auto sampleValue : buffer) {
        uint8_t sampleAbsValue = static_cast<uint8_t>(std::min(std::abs(sampleValue) * 255.0f, 255.0f));
        ++amplitudeHistogramm[sampleAbsValue];
    }

    size_t bufferSize = buffer.size();

    for (size_t n = 256; n > 0; n--) {
        float val = amplitudeHistogramm[n - 1] * 2500.0f / bufferSize;
        if (val > peakThreshold) {
            return static_cast<PCMTYPE>(n - 1) / 255;
        }
    }

    return 1;
}

void PcmNormalization::Normalize(std::vector<PCMTYPE>& buffer, PCMTYPE toAmplitude, NormalizationTypes normalizationType)
{
    auto maxSample = normalizationType == NormalizationTypes::Histogramm
                         ? GetAverageAmplitude(buffer, ADSSS::Config::HistogrammPeakThreshold)
                         : std::abs(
                               *std::max_element(buffer.begin(), buffer.end(), [](auto a, auto b) { return std::abs(a) > std::abs(b); }));

    auto amplifyBy = toAmplitude / maxSample;

    for (size_t n = 0; n < buffer.size(); n++) {
        buffer[n] *= amplifyBy;
    }
}

} // namespace PcmHelpers
} // namespace ADSSS
