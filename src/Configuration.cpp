#include "Configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace ADSSS {

/**** Default configuration ****/
bool Config::IsDebug = false;

unsigned Config::SamplerateDetectorStepsHz = 5;
int Config::SamplerateDetectorDownSteps = 2;
int Config::SamplerateDetectorUpSteps = 1;

unsigned Config::ExpectedReferenceSamplerateHz = 16000;
float Config::ReferenceNormalizationAmplitude = 0.9f;
float Config::HistogrammPeakThreshold = 0.03f;
FFT::WindowFunctionTypes Config::EncodingWindowFunction = FFT::WindowFunctionTypes::Square;
FFT::WindowFunctionTypes Config::DecodingWindowFunction = FFT::WindowFunctionTypes::Square;
unsigned Config::CorrelatorSliceDurationMilliseconds = 4000;
unsigned Config::CorrelatorPeriodMilliseconds = 1000;
float Config::CorrelatorThreshold = 250;
unsigned Config::DataDetectorFFTSize = 8192;
unsigned Config::DataDetectorLowerFrequencyHz = 100;
float Config::DataDetectorFilterValue = 0.3;
float Config::DataDetectorBeforeThresholdDb = -20;
float Config::DataDetectorAfterThresholdDb = 15;
unsigned Config::FFTSize = 65536 * 2;
unsigned Config::FIRTaps = 65536 * 2;
unsigned Config::StartChunkMarginMilliseconds = 800;
unsigned Config::DataChunkMilliseconds = 1100;
unsigned Config::BitsPerDataChunk = 256;
unsigned Config::BitSpreadPerDataChunk = 12;
unsigned Config::LowerUsableFrequencyHz = 1700;
float Config::BitBandwidthHz = 1.5;
float Config::BitEncodePaddingHz = 0.25;
float Config::BitDecodeMarginHz = 0.15;
float Config::EncoderBitGainPositiveLevelDb = 14;
float Config::EncoderBitGainNegativeLevelDb = -120;
const std::vector<uint8_t> Config::InitRandomVector{ 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
/********************************/

static const char* getValue(const char* line, const char* key)
{
    size_t keylen = strlen(key);

    if (strncmp(line, key, keylen) != 0) {
        return nullptr;
    }

    const char* val = line + keylen;
    if (*val != '=') {
        return nullptr;
    }

    if (!*++val) {
        return nullptr;
    }

    return val;
}

static bool tryParse(const char* line, const char* key, bool* val)
{
    auto v = getValue(line, key);

    if (!v) {
        return false;
    }

    if (strcmp(v, "true") == 0) {
        *val = true;
        return true;
    }

    if (strcmp(v, "false") == 0) {
        *val = false;
        return true;
    }

    return false;
}

static bool tryParse(const char* line, const char* key, unsigned* val)
{
    auto v = getValue(line, key);
    return v && sscanf(v, "%u", val) == 1;
}

static bool tryParse(const char* line, const char* key, float* val)
{
    auto v = getValue(line, key);
    return v && sscanf(v, "%f", val) == 1;
}

static void parseConfigLine(const char* line)
{
#define TRY_PARSE(param)                          \
    if (tryParse(line, #param, &Config::param)) { \
        return;                                   \
    }

    TRY_PARSE(IsDebug)
    TRY_PARSE(ExpectedReferenceSamplerateHz)
    TRY_PARSE(ReferenceNormalizationAmplitude)
    TRY_PARSE(HistogrammPeakThreshold)
    TRY_PARSE(CorrelatorSliceDurationMilliseconds)
    TRY_PARSE(CorrelatorPeriodMilliseconds)
    TRY_PARSE(CorrelatorThreshold)
    TRY_PARSE(DataDetectorFFTSize)
    TRY_PARSE(DataDetectorFilterValue)
    TRY_PARSE(DataDetectorBeforeThresholdDb)
    TRY_PARSE(DataDetectorAfterThresholdDb)
    TRY_PARSE(FFTSize)
    TRY_PARSE(FIRTaps)
    TRY_PARSE(StartChunkMarginMilliseconds)
    TRY_PARSE(DataChunkMilliseconds)
    TRY_PARSE(BitsPerDataChunk)
    TRY_PARSE(BitSpreadPerDataChunk)
    TRY_PARSE(BitBandwidthHz)
    TRY_PARSE(BitEncodePaddingHz)
    TRY_PARSE(BitDecodeMarginHz)
    TRY_PARSE(EncoderBitGainPositiveLevelDb)
    TRY_PARSE(EncoderBitGainNegativeLevelDb)

#undef TRY_PARSE
}

void Config::ParseConfigFile(const char* file)
{
    FILE* f = fopen(file, "r");
    if (!f) {
        return;
    }

    char* line = nullptr;
    size_t len = 0;

    while (!feof(f)) {
        ssize_t ret = getline(&line, &len, f);
        if (ret < 0) {
            free(line);
            return;
        }

        parseConfigLine(line);
    }

    free(line);
}

} // namespace ADSSS
