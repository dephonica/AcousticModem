#pragma once

#include <complex>
#include <iostream>
#include <vector>

#include "Configuration.h"
#include "kissfft/kiss_fftr.h"

namespace ADSSS {
namespace FFT {

class FftEngine
{
private:
    kiss_fftr_cfg forwardFft_, inverseFft_;

    std::vector<kiss_fft_cpx> kissBufferCpx_;
    std::vector<kiss_fft_scalar> kissBufferScalar_;

    std::vector<std::complex<PCMTYPE>> convolutionBuffer_;
    std::vector<std::complex<PCMTYPE>> convolutionKernel_;

public:
    explicit FftEngine(size_t fftSize);
    ~FftEngine();

    void ExecuteR2C(const std::vector<PCMTYPE>& realBuffer, std::vector<std::complex<PCMTYPE>>& complexBuffer);
    void ExecuteC2R(const std::vector<std::complex<PCMTYPE>>& complexBuffer, std::vector<PCMTYPE>& realBuffer);

    void SetConvolutionKernel(const std::vector<std::complex<PCMTYPE>>& complexKernel);
    void ExecuteConvolution(const std::vector<PCMTYPE>& realInput, std::vector<PCMTYPE>& realOutput);

    size_t GetFFTSize() const { return kissBufferScalar_.size(); }
    size_t GetComplexSize() const { return kissBufferCpx_.size(); }
};

} // namespace FFT
} // namespace ADSSS
