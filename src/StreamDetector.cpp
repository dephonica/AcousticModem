#include "StreamDetector.h"
#include "LogConversions.h"

#include <iostream>

namespace ADSSS {

StreamDetector::StreamDetector(unsigned sampleRate, int fftSize, float detectorFilterValue, float detectorBeforeThresholdDb, 
                                float detectorAfterThresholdDb)
    : isStreamDetected_(false)
    , sampleRate_(sampleRate)
    , higherUsableFrequencyHz_(Config::LowerUsableFrequencyHz + (Config::BitsPerDataChunk * Config::BitSpreadPerDataChunk) * Config::BitFullBandwidthHz())
    , fftHelper_(sampleRate, fftSize, Config::DecodingWindowFunction)
    , detectorFilterValue_(detectorFilterValue)
    , beforeDetectorValue_(0)
    , afterDetectorValue_(0)
    , detectorBeforeThresholdDb_(detectorBeforeThresholdDb)
    , detectorAfterThresholdDb_(detectorAfterThresholdDb)
    , samplesCollected_(0)
    , collectBuffer_(fftSize / 2)
{
}

void StreamDetector::PushChunk(const std::vector<PCMTYPE>& samplesData)
{
    fftHelper_.PushChunk(samplesData);

    auto energyBefore = fftHelper_.GetChunkLevel(Config::DataDetectorLowerFrequencyHz, Config::LowerUsableFrequencyHz);
    auto energyAfter = fftHelper_.GetChunkLevel(higherUsableFrequencyHz_, (sampleRate_ / 2 - higherUsableFrequencyHz_) - 100);
    auto energyIn = fftHelper_.GetChunkLevel(Config::LowerUsableFrequencyHz, higherUsableFrequencyHz_ - Config::LowerUsableFrequencyHz);

    auto energyBeforeDb = Math::LogConversions::ValueToDecibels(energyBefore);
    auto energyAfterDb = Math::LogConversions::ValueToDecibels(energyAfter);
    auto energyInDb = Math::LogConversions::ValueToDecibels(energyIn);

    auto currentDetectorBefore = energyInDb - energyBeforeDb;
    auto currentDetectorAfter = energyInDb - energyAfterDb;

    beforeDetectorValue_ = beforeDetectorValue_ * (1 - detectorFilterValue_) + currentDetectorBefore * detectorFilterValue_;
    afterDetectorValue_ = afterDetectorValue_ * (1 - detectorFilterValue_) + currentDetectorAfter * detectorFilterValue_;

    //std::cerr << "before " << beforeDetectorValue_ << ", after: " << afterDetectorValue_ << std::endl;
}

void StreamDetector::PushSamples(const std::vector<PCMTYPE>& samplesData, size_t offset, size_t length)
{
    size_t sourceOffset = offset;
    size_t samplesRemain = length;

    while (samplesRemain > 0) {
        size_t remainToCollect = collectBuffer_.size() - samplesCollected_;

        if (samplesRemain < remainToCollect) {
            remainToCollect = samplesRemain;
        }

        std::copy(samplesData.begin() + sourceOffset,
                  samplesData.begin() + sourceOffset + remainToCollect,
                  collectBuffer_.begin() + samplesCollected_);

        samplesCollected_ += remainToCollect;

        if (samplesCollected_ >= collectBuffer_.size()) {
            samplesCollected_ = 0;
            PushChunk(collectBuffer_);
        }

        sourceOffset += remainToCollect;
        samplesRemain -= remainToCollect;
    }
}

} // namespace ADSSS
