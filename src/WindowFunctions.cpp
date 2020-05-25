#include "WindowFunctions.h"

#include "MathDefines.h"

namespace ADSSS {
namespace FFT {

WindowFunctions::WindowFunctions(WindowFunctionTypes windowType, size_t windowSize)
{
    Reset(windowType, windowSize);
}

void WindowFunctions::Reset(WindowFunctionTypes windowType, size_t windowSize)
{
    windowType_ = windowType;

    if (windowType == WindowFunctionTypes::Blackman) {
        GenerateBlackman(windowSize);
    }
    else if (windowType == WindowFunctionTypes::Hamming) {
        GenerateHamming(windowSize);
    }
    else if (windowType == WindowFunctionTypes::Square) {
        GenerateSquare(windowSize);
    }
}

void WindowFunctions::GenerateBlackman(size_t windowSize)
{
    size_t N = windowSize;

    double a = 0.16;
    double a0 = (1 - a) / 2;
    double a1 = 0.5;
    double a2 = a / 2;

    double scale = 0;

    windowData_.clear();

    for (size_t n = 0; n < N; n++) {
        double w = a0 - a1 * std::cos(2 * M_PI * n / (N - 1)) + a2 * std::cos(4 * M_PI * n / (N - 1));
        windowData_.push_back(static_cast<float>(w));

        scale += w;
    }

    scale_ = 1.0 / (scale / N);
}

void WindowFunctions::GenerateHamming(size_t windowSize)
{
    size_t N = windowSize;

    double a0 = 0.5;
    double a1 = 1 - a0;

    double scale = 0;

    windowData_.clear();

    for (size_t n = 0; n < N; n++) {
        double w = a0 - a1 * std::cos(2 * M_PI * n / (N - 1));
        windowData_.push_back(static_cast<float>(w));

        scale += w;
    }

    scale_ = 1.0 / (scale / N);
}

void WindowFunctions::GenerateSquare(size_t windowSize)
{
    windowData_ = std::vector<PCMTYPE>(windowSize, 1);
    scale_ = 1.0;
}

void WindowFunctions::Apply(std::vector<PCMTYPE>& processingBuffer, PCMTYPE gain)
{
    for (size_t n = 0; n < windowData_.size(); n++) {
        processingBuffer[n] = processingBuffer[n] * (windowData_[n] * gain);
    }
}

} // namespace FFT
} // namespace ADSSS
