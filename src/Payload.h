#pragma once

#include <vector>

#include "Configuration.h"

#include "reedsolomon/rs"

namespace ADSSS {

class Payload
{
private:
    ezpwd::RS<Config::ReedSolomonBlockSize, Config::ReedSolomonPayloadSize> reedSolomon_;

    int errorsCorrected_ = -1;

public:
    std::vector<uint8_t> Serialize(const std::vector<uint8_t>& rawPayload);
    std::vector<uint8_t> Deserialize(const std::vector<uint8_t>& serializedData);

    int GetErrorsCorrected() { return errorsCorrected_; }
};

} // namespace ADSSS
