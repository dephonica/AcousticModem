#pragma once

#include "Configuration.h"

#include "IStreamCallbacks.h"
#include "StreamFrontend.h"
#include "StreamDetector.h"

namespace ADSSS {

class StreamDecoder : public IStreamCallbacks
{
private:
    const ADSSS::PcmHelpers::IReferenceSource& referenceSource_;
    StreamFrontend streamFrontend_;

    IStreamCallbacks& streamCallbacks_;

    StreamDetector streamDetector_;

public:
    StreamDecoder(const ADSSS::PcmHelpers::IReferenceSource& referenceData, IStreamCallbacks& streamCallbacks);

    bool PushSamples(const std::vector<PCMTYPE>& samplesData, size_t offset, size_t length)
    {
        streamDetector_.PushSamples(samplesData, offset, length);
        streamFrontend_.PushSamples(samplesData, offset, length);

        return streamDetector_.IsDetected();
    }

    void OnCorrelatorTriggered(const std::vector<PCMTYPE>& samplesData, size_t samplesCount) override;
    void OnDataDecoded(const std::vector<uint8_t>& /*payloadData*/) override {}
};

} // namespace ADSSS
