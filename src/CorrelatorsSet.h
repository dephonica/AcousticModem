#pragma once

#include <memory>
#include <vector>

#include "Configuration.h"

#include "IReferenceSource.h"
#include "Correlator2.h"

namespace ADSSS {

class CorrelatorsSet
{
private:
    const PcmHelpers::IReferenceSource& referenceSource_;

    std::vector<std::unique_ptr<Correlator2>> correlators_;

    void InitializeCorrelators(size_t sliceDurationMs);

public:
    CorrelatorsSet(const PcmHelpers::IReferenceSource& referenceSource, size_t sliceDurationMs);
    CorrelationResult Calculate(const std::vector<PCMTYPE>& samplesData, size_t offset = 0);
};

} // namespace ADSSS
