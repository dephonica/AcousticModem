#include "Correlator2.h"
#include "LogConversions.h"

#include <assert.h>

namespace ADSSS {

Correlator2::Correlator2(const ADSSS::PcmHelpers::IReferenceSource& referenceData, size_t referenceLengthMs)
    : sampleRate_(referenceData.GetSampleRate())
    , referenceSamples_(std::min(sampleRate_ * referenceLengthMs / 1000, referenceData.GetReferenceData().size()))
    , fftEngine_(GetFFTSize(referenceSamples_ * 2 - 1))
    , processingBuffer_(fftEngine_.GetFFTSize())
    , complexBuffer_(fftEngine_.GetComplexSize())
{
    InitializeReference(referenceData);
}

void Correlator2::InitializeReference(const ADSSS::PcmHelpers::IReferenceSource& referenceData)
{
    auto& referencePcm = referenceData.GetReferenceData();

    for (size_t n = 0, r = fftEngine_.GetFFTSize() / 2; n < referenceSamples_; n++, r--) {
        processingBuffer_[r] = referencePcm[n];
    }

    std::vector<std::complex<PCMTYPE>> complexReference(fftEngine_.GetComplexSize());

    fftEngine_.ExecuteR2C(processingBuffer_, complexReference);
    fftEngine_.SetConvolutionKernel(complexReference);
}

CorrelationResult Correlator2::Calculate(const std::vector<PCMTYPE>& samplesData, size_t offset)
{
    size_t samplesToCopy = std::min(samplesData.size() - offset, referenceSamples_);
    std::copy(samplesData.begin() + offset, samplesData.begin() + offset + samplesToCopy, processingBuffer_.begin());

    if (samplesToCopy < processingBuffer_.size()) {
        std::fill(processingBuffer_.begin() + samplesToCopy, processingBuffer_.end(), 0);
    }

    fftEngine_.ExecuteConvolution(processingBuffer_, processingBuffer_);

    return FindOffsets(processingBuffer_);
}

CorrelationResult Correlator2::FindOffsets(const std::vector<PCMTYPE>& samplesData)
{
    size_t maxOffset = 0;
    PCMTYPE maxValue = -99999;

    for (size_t n = 0; n < samplesData.size(); n++) {
        if (samplesData[n] > maxValue) {
            maxValue = samplesData[n];
            maxOffset = n;
        }
    }

    maxOffset = maxOffset - fftEngine_.GetFFTSize() / 2;

    return CorrelationResult(maxOffset, maxValue);
}

} // namespace ADSSS
