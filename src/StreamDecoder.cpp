#include "StreamDecoder.h"

#include "Decoder.h"
#include "ReferenceSource.h"

namespace ADSSS {

StreamDecoder::StreamDecoder(const ADSSS::PcmHelpers::IReferenceSource& referenceSource, IStreamCallbacks& streamCallbacks)
    : referenceSource_(referenceSource)
    , streamFrontend_(referenceSource, *this, Config::CorrelatorPeriodMilliseconds, Config::CorrelatorSliceDurationMilliseconds, Config::CorrelatorThreshold)
    , streamCallbacks_(streamCallbacks)
    , streamDetector_(referenceSource.GetSampleRate(), Config::DataDetectorFFTSize, Config::DataDetectorFilterValue, Config::DataDetectorBeforeThresholdDb, Config::DataDetectorAfterThresholdDb)
{
}

void StreamDecoder::OnCorrelatorTriggered(const std::vector<PCMTYPE>& samplesData, size_t samplesCount)
{
    ADSSS::PcmHelpers::ReferenceSource encodedSource(referenceSource_.GetSampleRate(), samplesData, samplesCount);

    streamCallbacks_.OnCorrelatorTriggered(encodedSource.GetReferenceData(), samplesCount);

    ADSSS::Decoder decoder(encodedSource, referenceSource_);

    auto decodedData = decoder.Go();

    streamCallbacks_.OnDataDecoded(decodedData);
}

} // namespace ADSSS
