#include "Encoder.h"

#include "FirBlockConvolver.h"
#include "FirKernelSource.h"

namespace ADSSS {

Encoder::Encoder(const PcmHelpers::IReferenceSource& referenceSource)
    : referenceSource_(referenceSource)
    , fftHelper_(referenceSource.GetSampleRate(), ADSSS::Config::FFTSize, ADSSS::Config::DecodingWindowFunction)
    , envelopeGenerator_(referenceSource.GetSampleRate(), ADSSS::Config::FIRTaps)
{
}

std::vector<PCMTYPE> Encoder::Go(const std::vector<uint8_t>& payloadBytes)
{
    auto randomizedPayload = RandomizePayload(payloadBytes);
    return EncodeData(randomizedPayload);
}

std::vector<uint8_t> Encoder::RandomizePayload(const std::vector<uint8_t>& payloadBytes)
{
    std::vector<uint8_t> randomizedPayload;

    RandomGenerator randomGenerator(ADSSS::Config::InitRandomVector);

    for (auto b : payloadBytes) {
        randomizedPayload.push_back(b ^ randomGenerator.GetNextByte());
    }

    return randomizedPayload;
}

std::vector<PCMTYPE> Encoder::EncodeData(const std::vector<uint8_t>& payloadBytes)
{
    Bitwise::BitBuffer bitBuffer;
    bitBuffer.PushBytes(payloadBytes);

    auto referenceChunksCount = referenceSource_.GetReferenceChunksCount();

    auto prefixSamples = referenceSource_.GetPrefixSamples();

    std::vector<PCMTYPE> resultData(prefixSamples.size() + referenceChunksCount * referenceSource_.GetSamplesPerChunk());

    std::copy(prefixSamples.begin(), prefixSamples.end(), resultData.begin());

    for (size_t chunkIndex = 0; chunkIndex < referenceChunksCount; chunkIndex++) {
        auto chunkData = referenceSource_.GetChunkData(chunkIndex);
        ApplyDataToChunk(chunkData, bitBuffer);
        PutChunkData(resultData, chunkIndex, chunkData);
    }

    resultData.resize(referenceSource_.GetReferenceData().size());

    return resultData;
}

void Encoder::ApplyDataToChunk(std::vector<PCMTYPE>& chunkData, Bitwise::BitBuffer& bitBuffer)
{
    if (bitBuffer.IsEmpty()) {
        return;
    }

    auto messageBits = bitBuffer.PopBits(ADSSS::Config::BitsPerDataChunk);

    fftHelper_.PushChunk(chunkData);

    envelopeGenerator_.PushMessage(messageBits, fftHelper_);

    ADSSS::FIR::FirKernelSource kernelSource(envelopeGenerator_.GetSampleRate(), envelopeGenerator_.GetEnvelopePoints());
    ADSSS::FIR::FirBlockConvolver firConvolver(kernelSource, chunkData.size());

    firConvolver.Convolve(chunkData, chunkData);
}

void Encoder::PutChunkData(std::vector<PCMTYPE>& resultData, size_t chunkIndex, const std::vector<PCMTYPE>& chunkData)
{
    std::copy(chunkData.begin(), chunkData.end(), resultData.begin() + referenceSource_.GetPrefixSamplesCount() + chunkIndex * referenceSource_.GetSamplesPerChunk());
}

} // namespace ADSSS
