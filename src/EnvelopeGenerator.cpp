#include "EnvelopeGenerator.h"

#include "LogConversions.h"

namespace ADSSS {
namespace Encoding {

EnvelopeGenerator::EnvelopeGenerator(unsigned sampleRate, unsigned taps)
    : sampleRate_(sampleRate)
    , hertzPerPoint_(float(sampleRate) / taps)
    , windowEngine_(ADSSS::Config::EncodingWindowFunction, 1)
    , envelopePoints_(taps / 2 + 1)
{
}

void EnvelopeGenerator::PushMessage(const std::vector<uint8_t>& sourceMessageBits,
                                                    const ADSSS::FFT::FftEngineHelper& sourceChunk)
{
    if (ADSSS::Config::IsDebug && sourceMessageBits.size() > ADSSS::Config::BitsPerDataChunk) {
        std::cerr << "Invalid bit message pushed into EnvelopeGenerator: " << ADSSS::Config::BitsPerDataChunk << " bits are expected but "
                  << sourceMessageBits.size() << " bits was pushed";
    }

    if (ADSSS::Config::IsDebug && (ADSSS::Config::BitSpreadPerDataChunk < 4 || ADSSS::Config::BitSpreadPerDataChunk % 4 != 0)) {
        std::cerr << "Invalid BitSpreadPerDataChunk value for selected Fir4BitsInterleaved engine";
    }

    for (size_t n = 0; n < envelopePoints_.size(); n++) {
        auto& envelopePoint = envelopePoints_[n];

        envelopePoint.Frequency = n * hertzPerPoint_;
        envelopePoint.Gain = 1.0;
        envelopePoint.Phase = 0.0;
    }

    for (size_t n = 0; n < ADSSS::Config::BitsPerDataChunk; n++) {
        uint8_t bitValue = n < sourceMessageBits.size() ? sourceMessageBits[n] : 0;

        for (size_t spreadIndex = 0; spreadIndex < ADSSS::Config::BitSpreadPerDataChunk / 4; spreadIndex++) {
            auto chunkIndex = n * 4 + spreadIndex * ADSSS::Config::BitsPerDataChunk * 4;

            if (bitValue == 1) {
                SetChunkGain(chunkIndex + 0, ADSSS::Config::EncoderBitGainPositiveLevelDb);
                SetChunkGain(chunkIndex + 1, ADSSS::Config::EncoderBitGainNegativeLevelDb);
                SetChunkGain(chunkIndex + 2, ADSSS::Config::EncoderBitGainNegativeLevelDb);
                SetChunkGain(chunkIndex + 3, ADSSS::Config::EncoderBitGainPositiveLevelDb);
            }
            else {
                SetChunkGain(chunkIndex + 0, ADSSS::Config::EncoderBitGainNegativeLevelDb);
                SetChunkGain(chunkIndex + 1, ADSSS::Config::EncoderBitGainPositiveLevelDb);
                SetChunkGain(chunkIndex + 2, ADSSS::Config::EncoderBitGainPositiveLevelDb);
                SetChunkGain(chunkIndex + 3, ADSSS::Config::EncoderBitGainNegativeLevelDb);
            }
        }
    }
}

void EnvelopeGenerator::SetChunkGain(size_t chunkIndex, float gainDb)
{
    float chunkStartFreq = ADSSS::Config::Config::LowerUsableFrequencyHz + chunkIndex * ADSSS::Config::BitFullBandwidthHz();

    float bitStartFreq = chunkStartFreq + ADSSS::Config::BitEncodePaddingHz;
    float bitEndFreq = bitStartFreq + ADSSS::Config::BitBandwidthHz;

    size_t bitStartIndex = static_cast<size_t>(bitStartFreq / hertzPerPoint_);
    size_t bitEndIndex = static_cast<size_t>(bitEndFreq / hertzPerPoint_);

    ApplyGain(bitStartIndex, bitEndIndex, gainDb);
}

void EnvelopeGenerator::ApplyGain(size_t startIndex, size_t endIndex, float gainDb)
{
    size_t windowSize = endIndex - startIndex;
    if (windowEngine_.GetWindowSize() < windowSize) {
        windowEngine_.Reset(ADSSS::Config::EncodingWindowFunction, windowSize);
    }

    auto gainValue = ADSSS::Math::LogConversions::DecibelsToValue(gainDb);

    auto& windowData = windowEngine_.GetWindowData();

    for (size_t n = startIndex, windowIndex = 0; n < endIndex; n++, windowIndex++) {
        float windowGain = windowData[windowIndex];

        if (gainDb < 0) {
            envelopePoints_[n].Gain = 1 - (1 - gainValue) * windowGain;
        }
        else {
            envelopePoints_[n].Gain = gainValue * windowGain;
        }
    }
}

} // namespace Encoding
} // namespace ADSSS
