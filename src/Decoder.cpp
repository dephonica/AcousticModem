#include "Decoder.h"

#include "LogConversions.h"

#include <algorithm>

namespace ADSSS {

Decoder::Decoder(const PcmHelpers::IReferenceSource& encodedSource, const PcmHelpers::IReferenceSource& referenceSource)
    : encodedSource_(encodedSource)
    , referenceSource_(referenceSource)
{
}

std::vector<uint8_t> Decoder::Go()
{
    std::vector<uint8_t> messageBuffer;

    ADSSS::FFT::FftEngineHelper fftHelperCaptured(encodedSource_.GetSampleRate(), ADSSS::Config::FFTSize, ADSSS::Config::DecodingWindowFunction);

    size_t encodedChunksCount = encodedSource_.GetReferenceChunksCount() - 1;

    for (size_t chunkIndex = 0; chunkIndex < encodedChunksCount; chunkIndex++) {
        auto capturedChunk = encodedSource_.GetChunkData(chunkIndex);

        fftHelperCaptured.PushChunk(capturedChunk);

        auto chunkData = DecodeChunk(fftHelperCaptured);

        messageBuffer.insert(messageBuffer.end(), chunkData.begin(), chunkData.end());
    }

    DerandomizePayload(messageBuffer);

    return messageBuffer;
}

void Decoder::DerandomizePayload(std::vector<uint8_t>& payloadBytes)
{
    RandomGenerator randomGenerator(ADSSS::Config::InitRandomVector);

    for (size_t i = 0; i < payloadBytes.size(); ++i) {
        payloadBytes[i] ^= randomGenerator.GetNextByte();
    }
}

std::vector<uint8_t> Decoder::DecodeChunk(ADSSS::FFT::FftEngineHelper& fftHelperCaptured)
{
    auto capturedPowers = MeasureChunkData(fftHelperCaptured);

    ADSSS::Bitwise::BitBuffer bitVector;

    double snrLevel = 0.0;
    double minSnrLevel = 999999;
    double maxSnrLevel = -999999;

    for (size_t n = 0; n < ADSSS::Config::BitsPerDataChunk; n++) {
        double bitPowerAccumulator = 0;

        for (size_t spreadIndex = 0; spreadIndex < ADSSS::Config::BitSpreadPerDataChunk / 4; spreadIndex++) {
            auto index = n + spreadIndex * ADSSS::Config::BitsPerDataChunk;
            bitPowerAccumulator += capturedPowers[index];
        }

        uint8_t bitValue = bitPowerAccumulator > 0 ? 1 : 0;

        double absBitLevel = std::abs(bitPowerAccumulator);
        snrLevel += absBitLevel;
        minSnrLevel = std::min(minSnrLevel, absBitLevel);
        maxSnrLevel = std::max(maxSnrLevel, absBitLevel);

        bitVector.PushBit(bitValue);
    }

    snrLevel /= ADSSS::Config::BitsPerDataChunk;

    std::cerr << "Total SNR: " << snrLevel << ", min SNR: " << minSnrLevel << ", max SNR: " << maxSnrLevel << std::endl;

    return bitVector.GetBytes();
}

std::vector<double> Decoder::MeasureChunkData(ADSSS::FFT::FftEngineHelper& fftHelperCaptured)
{
    std::vector<double> resultLevels(ADSSS::Config::BitsPerDataChunk * ADSSS::Config::BitSpreadPerDataChunk / 4);

    for (size_t n = 0; n < ADSSS::Config::BitsPerDataChunk; n++) {
        for (size_t spreadIndex = 0; spreadIndex < ADSSS::Config::BitSpreadPerDataChunk / 4; spreadIndex++) {
            auto chunkIndex = n * 4 + spreadIndex * ADSSS::Config::BitsPerDataChunk * 4;

            auto powerPositiveCaptured1 = ADSSS::Math::LogConversions::ValueToDecibels(fftHelperCaptured.GetChunkLevel(chunkIndex + 0));
            auto powerNegativeCaptured1 = ADSSS::Math::LogConversions::ValueToDecibels(fftHelperCaptured.GetChunkLevel(chunkIndex + 1));

            auto powerPositiveCaptured2 = ADSSS::Math::LogConversions::ValueToDecibels(fftHelperCaptured.GetChunkLevel(chunkIndex + 2));
            auto powerNegativeCaptured2 = ADSSS::Math::LogConversions::ValueToDecibels(fftHelperCaptured.GetChunkLevel(chunkIndex + 3));

            auto powerDiff1 = powerPositiveCaptured1 - powerNegativeCaptured1;
            auto powerDiff2 = powerPositiveCaptured2 - powerNegativeCaptured2;

            auto resultIndex = n + spreadIndex * ADSSS::Config::BitsPerDataChunk;

            resultLevels[resultIndex] = powerDiff1 - powerDiff2;
        }
    }

    return resultLevels;
}

} // namespace ADSSS
