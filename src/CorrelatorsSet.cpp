#include "CorrelatorsSet.h"

#include "PcmResampling.h"
#include "ReferenceSource.h"

namespace ADSSS {

CorrelatorsSet::CorrelatorsSet(const PcmHelpers::IReferenceSource& referenceSource, size_t sliceDurationMs)
    : referenceSource_(referenceSource)
{
    InitializeCorrelators(sliceDurationMs);
}

void CorrelatorsSet::InitializeCorrelators(size_t sliceDurationMs)
{
    unsigned sampleRate = Config::ExpectedReferenceSamplerateHz - 
        Config::SamplerateDetectorDownSteps * Config::SamplerateDetectorStepsHz;

    auto& referenceSamples = referenceSource_.GetReferenceData();
    std::vector<PCMTYPE> referenceResampled(referenceSamples.size());

    for (int n = -Config::SamplerateDetectorDownSteps; 
            n <= Config::SamplerateDetectorUpSteps; 
            n++, sampleRate += Config::SamplerateDetectorStepsHz)
    {
        double ratio = sampleRate / static_cast<double>(Config::ExpectedReferenceSamplerateHz);

        PcmHelpers::PcmResampling::Resample(referenceSamples, 0, referenceResampled, 0, referenceSamples.size(), ratio);

        PcmHelpers::ReferenceSource resampledSource(Config::ExpectedReferenceSamplerateHz, referenceResampled);

        correlators_.push_back(std::make_unique<Correlator2>(resampledSource, sliceDurationMs));
    }
}

CorrelationResult CorrelatorsSet::Calculate(const std::vector<PCMTYPE>& samplesData, size_t offset)
{
    CorrelationResult bestCorrelationResult;

    unsigned sampleRate = Config::ExpectedReferenceSamplerateHz - 
        Config::SamplerateDetectorDownSteps * Config::SamplerateDetectorStepsHz;

    for (auto& correlator : correlators_)
    {
        auto correlationResult = correlator->Calculate(samplesData, offset);

        if (correlationResult.correlation_ > bestCorrelationResult.correlation_)
        {
            bestCorrelationResult = correlationResult;
            bestCorrelationResult.sampleRate_ = sampleRate;
        }

        sampleRate += Config::SamplerateDetectorStepsHz;
    }

    return bestCorrelationResult;
}

} // namespace ADSSS
