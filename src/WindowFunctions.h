#pragma once

#include <stddef.h>
#include <vector>

#include "Configuration.h"
#include "WindowFunctionTypes.h"

namespace ADSSS {
namespace FFT {

class WindowFunctions
{
private:
    WindowFunctionTypes windowType_;

    double scale_;
    std::vector<PCMTYPE> windowData_;

    void GenerateSquare(size_t windowSize);
    void GenerateHamming(size_t windowSize);
    void GenerateBlackman(size_t windowSize);

public:
    WindowFunctions(WindowFunctionTypes windowType, size_t windowSize);

    void Reset(WindowFunctionTypes windowType, size_t windowSize);

    void Apply(std::vector<PCMTYPE>& processingBuffer, PCMTYPE gain = 1.0);

    size_t GetWindowSize() const { return windowData_.size(); }

    WindowFunctionTypes GetWindowType() const { return windowType_; }

    double GetScale() const { return scale_; }

    const std::vector<PCMTYPE>& GetWindowData() const { return windowData_; }

    //PCMTYPE GetAverage() const { return 1.0f / _scale; }
};

} // namespace FFT
} // namespace ADSSS
