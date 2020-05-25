#pragma once

#include "Configuration.h"

#include "CorrelatorsSet.h"
#include "IReferenceSource.h"
#include "IStreamCallbacks.h"

namespace ADSSS {

class StreamFrontend
{
private:
    unsigned sampleRate_;
    size_t referenceSamplesCount_;

    IStreamCallbacks& streamCallbacks_;

    CorrelatorsSet correlators_;
    int correlatorThreshold_;

    size_t samplesCollected_;
    std::vector<PCMTYPE> collectBuffer_;

    std::vector<PCMTYPE> samplesBuffer_;
    std::vector<PCMTYPE> normalizeBuffer_;

    void InitReferenceFootprint(const ADSSS::PcmHelpers::IReferenceSource& referenceData);
    void ProcessStreamChunk();

public:
    StreamFrontend(const ADSSS::PcmHelpers::IReferenceSource& referenceData, 
                   IStreamCallbacks& streamCallbacks, 
                   size_t correlationCheckPeriodMs, 
                   size_t sliceDurationMs, 
                   int correlatorThreshold);

    void PushSamples(const std::vector<PCMTYPE>& samplesData, size_t offset, size_t length);
};

} // namespace ADSSS
