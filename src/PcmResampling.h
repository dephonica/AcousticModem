#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include "Configuration.h"

#include <stdio.h>
#include <stdlib.h>

namespace ADSSS {
namespace PcmHelpers {

class PcmResampling
{
private:
public:
    static void Resample(const std::vector<PCMTYPE>& sourceSamples,
                         int sourceOffset,
                         std::vector<PCMTYPE>& targetSamples,
                         int targetOffset,
                         int sourceSamplesCount,
                         double ratio)
    {
        size_t targetSampleRate = Config::ExpectedReferenceSamplerateHz * ratio;
        size_t targetSamplesCount = sourceSamplesCount * ratio;

        auto sourceBuffer = new PCMTYPE[sourceSamplesCount];
        auto targetBuffer = new PCMTYPE[targetSamplesCount + 1];

        std::copy(sourceSamples.begin() + sourceOffset, sourceSamples.begin() + sourceOffset + sourceSamplesCount, sourceBuffer);

        struct SwrContext* swrContext = swr_alloc();
        av_opt_set_int(swrContext, "in_channel_layout", AV_CH_LAYOUT_MONO, 0);
        av_opt_set_int(swrContext, "in_sample_rate", Config::ExpectedReferenceSamplerateHz, 0);
        av_opt_set_sample_fmt(swrContext, "in_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
        av_opt_set_int(swrContext, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
        av_opt_set_int(swrContext, "out_sample_rate", targetSampleRate, 0);
        av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

        if (swr_init(swrContext) >= 0)
        {
            auto resampled = swr_convert(swrContext,
                                         reinterpret_cast<uint8_t**>(&targetBuffer),
                                         targetSamplesCount,
                                         const_cast<const uint8_t**>(reinterpret_cast<uint8_t**>(&sourceBuffer)),
                                         sourceSamplesCount);

            PCMTYPE* tailPointer = &targetBuffer[resampled];
            resampled += swr_convert(swrContext, reinterpret_cast<uint8_t**>(&tailPointer), targetSamplesCount - resampled, NULL, 0);

            size_t samplesToCopy = std::min(targetSamplesCount, targetSamples.size() - targetOffset);

            std::copy(targetBuffer, targetBuffer + samplesToCopy, targetSamples.begin() + targetOffset);
        }
        else
        {
            std::cerr << "Failed to initialize the resampling context" << std::endl;
        }

        swr_free(&swrContext);

        delete[] sourceBuffer;
        delete[] targetBuffer;
    }
};

} // namespace PcmHelpers
} // namespace ADSSS
