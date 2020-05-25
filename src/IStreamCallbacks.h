#pragma once

#include <stddef.h>

#include "Configuration.h"

namespace ADSSS {

class IStreamCallbacks
{
public:
    virtual void OnCorrelatorTriggered(const std::vector<PCMTYPE>& samplesData, size_t samplesCount) = 0;
    virtual void OnDataDecoded(const std::vector<uint8_t>& payloadData) = 0;

    virtual ~IStreamCallbacks() = default;
};

} // namespace ADSSS
