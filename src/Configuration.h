#pragma once

#include <vector>

#include <inttypes.h>

#include "WindowFunctionTypes.h"

#define PCMTYPE float

namespace ADSSS {

struct Config
{
    static bool IsDebug;

    /* Reference source section */
    static unsigned ExpectedReferenceSamplerateHz;
    static float ReferenceNormalizationAmplitude;
    static float HistogrammPeakThreshold;

    /* Samplerate detector settings */
    static unsigned SamplerateDetectorStepsHz;
    static int SamplerateDetectorDownSteps;
    static int SamplerateDetectorUpSteps;

    /* Algo settings */
    static const unsigned ReedSolomonBlockSize = 255;
    static const unsigned ReedSolomonPayloadSize = 233;

    static FFT::WindowFunctionTypes EncodingWindowFunction;
    static FFT::WindowFunctionTypes DecodingWindowFunction;

    static unsigned CorrelatorSliceDurationMilliseconds;
    static unsigned CorrelatorPeriodMilliseconds;
    static float CorrelatorThreshold;

    static unsigned DataDetectorFFTSize;
    static unsigned DataDetectorLowerFrequencyHz;
    static float DataDetectorFilterValue;
    static float DataDetectorBeforeThresholdDb;
    static float DataDetectorAfterThresholdDb;

    static unsigned FFTSize;
    static unsigned FIRTaps;

    static unsigned StartChunkMarginMilliseconds;
    static unsigned DataChunkMilliseconds;
    static unsigned BitsPerDataChunk;
    static unsigned BitSpreadPerDataChunk;

    static unsigned LowerUsableFrequencyHz;

    static float BitBandwidthHz;
    static float BitEncodePaddingHz;
    static float BitDecodeMarginHz;

    static float BitFullBandwidthHz() { return BitBandwidthHz + BitEncodePaddingHz * 2; }

    static float EncoderBitGainPositiveLevelDb;
    static float EncoderBitGainNegativeLevelDb;

    /* Misc settings */
    static const std::vector<uint8_t> InitRandomVector;

    static void ParseConfigFile(const char*);
};

} // namespace ADSSS
