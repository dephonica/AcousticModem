#include "StreamFrontend.h"
#include "PcmNormalization.h"
#include "PcmResampling.h"

namespace ADSSS {

StreamFrontend::StreamFrontend(const ADSSS::PcmHelpers::IReferenceSource& referenceData,
                               IStreamCallbacks& streamCallbacks,
                               size_t correlationCheckPeriodMs,
                               size_t sliceDurationMs, 
                               int correlatorThreshold)
    : sampleRate_(referenceData.GetSampleRate())
    , referenceSamplesCount_(referenceData.GetReferenceData().size())
    , streamCallbacks_(streamCallbacks)
    , correlators_(referenceData, sliceDurationMs)
    , correlatorThreshold_(correlatorThreshold)
    , samplesCollected_(0)
    , collectBuffer_(referenceData.GetSampleRate() * correlationCheckPeriodMs / 1000)
    , samplesBuffer_(referenceSamplesCount_ + collectBuffer_.size())
    , normalizeBuffer_(samplesBuffer_.size())
{
}

void StreamFrontend::PushSamples(const std::vector<PCMTYPE>& samplesData, size_t offset, size_t length)
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
            ProcessStreamChunk();
        }

        sourceOffset += remainToCollect;
        samplesRemain -= remainToCollect;
    }
}

void StreamFrontend::ProcessStreamChunk()
{
    // move back samples buffer
    std::copy(samplesBuffer_.begin() + collectBuffer_.size(), samplesBuffer_.end(), samplesBuffer_.begin());

    // append chunk samples to the samples buffer
    std::copy(collectBuffer_.begin(), collectBuffer_.end(), samplesBuffer_.begin() + samplesBuffer_.size() - collectBuffer_.size());
    std::copy(samplesBuffer_.begin(), samplesBuffer_.end(), normalizeBuffer_.begin());

    ADSSS::PcmHelpers::PcmNormalization::Normalize(normalizeBuffer_,
                                                   Config::ReferenceNormalizationAmplitude,
                                                   ADSSS::PcmHelpers::NormalizationTypes::Histogramm);

    auto correlationResult = correlators_.Calculate(normalizeBuffer_);

    size_t bestOffset = correlationResult.offset_;
    float bestCorrelation = correlationResult.correlation_;

    if (bestCorrelation >= correlatorThreshold_ && 
        bestCorrelation < 100000 &&
        bestOffset <= collectBuffer_.size()) {
        std::cerr << "Correlation at: " << bestOffset << " with value: " << bestCorrelation << 
            " at sample rate: " << correlationResult.sampleRate_ << std::endl << std::endl;

        double ratio = static_cast<double>(Config::ExpectedReferenceSamplerateHz) / correlationResult.sampleRate_;

        std::vector<PCMTYPE> resampledSamples(referenceSamplesCount_);

        PcmHelpers::PcmResampling::Resample(samplesBuffer_, bestOffset, resampledSamples, 0, referenceSamplesCount_, ratio);

        streamCallbacks_.OnCorrelatorTriggered(resampledSamples, referenceSamplesCount_);
    }
}

} // namespace ADSSS
