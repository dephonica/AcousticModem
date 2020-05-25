#include "FftEngineFftw.h"

#include <algorithm>
#include <assert.h>

namespace ADSSS {
namespace FFT {

FftEngine::FftEngine(size_t fftSize)
    : cpuRealBuffer_(fftSize)
    , cpuComplexBuffer_(fftSize / 2 + 1)
    , convolutionBuffer_(fftSize / 2 + 1)
    , convolutionKernel_(fftSize)
{
    planForwardCpu_ = fftwf_plan_dft_r2c_1d(fftSize, cpuRealBuffer_.data(),
                            cpuComplexBuffer_.data(), FFTW_ESTIMATE | FFTW_PATIENT);
    planBackwardCpu_ = fftwf_plan_dft_c2r_1d(fftSize, cpuComplexBuffer_.data(),
                            cpuRealBuffer_.data(), FFTW_ESTIMATE | FFTW_PATIENT);
}

FftEngine::~FftEngine()
{
    fftwf_destroy_plan(planForwardCpu_);
    fftwf_destroy_plan(planBackwardCpu_);
}

static void copyToFftwBuffer(const std::vector<PCMTYPE>& in, std::vector<float>& out)
{
    auto samplesToCopy = std::min(in.size(), out.size());
    out.assign(in.begin(), in.begin() + samplesToCopy);

    while (samplesToCopy < out.size())
    {
        out[samplesToCopy] = 0;
        ++samplesToCopy;
    }
}

static void copyToFftwBuffer(const std::vector<std::complex<PCMTYPE>>& in, std::vector<fftwf_complex>& out)
{
    size_t n = 0;
    while (n < std::min(in.size(), out.size()))
    {
        out[n][0] = in[n].real();
        out[n][1] = in[n].imag();
        ++n;
    }

    while (n < out.size())
    {
        out[n][0] = 0;
        out[n][1] = 0;
        ++n;
    }
}

static void copyFromFftwBuffer(const std::vector<float>& in, std::vector<PCMTYPE>& out)
{
    auto samplesToCopy = std::min(in.size(), out.size());
    out.assign(in.begin(), in.begin() + samplesToCopy);

    while (samplesToCopy < out.size())
    {
        out[samplesToCopy] = 0;
        ++samplesToCopy;
    }
}

static void copyFromFftwBuffer(const std::vector<fftwf_complex>& in, std::vector<std::complex<PCMTYPE>>& out)
{
    size_t n = 0;
    while (n < std::min(in.size(), out.size()))
    {
        out[n].real(in[n][0]);
        out[n].imag(in[n][1]);
        ++n;
    }

    while (n < out.size())
    {
        out[n].real(0);
        out[n].imag(0);
        ++n;
    }
}

void FftEngine::ExecuteR2C(const std::vector<PCMTYPE>& realBuffer, std::vector<std::complex<PCMTYPE>>& complexBuffer)
{
    copyToFftwBuffer(realBuffer, cpuRealBuffer_);
    fftwf_execute(planForwardCpu_);
    copyFromFftwBuffer(cpuComplexBuffer_, complexBuffer);
}

void FftEngine::ExecuteC2R(const std::vector<std::complex<PCMTYPE>>& complexBuffer, std::vector<PCMTYPE>& realBuffer)
{
    copyToFftwBuffer(complexBuffer, cpuComplexBuffer_);
    fftwf_execute(planBackwardCpu_);
    copyFromFftwBuffer(cpuRealBuffer_, realBuffer);
}

void FftEngine::SetConvolutionKernel(const std::vector<std::complex<PCMTYPE>>& complexKernel)
{
    convolutionKernel_.assign(complexKernel.begin(), complexKernel.end());
}

void FftEngine::ExecuteConvolution(const std::vector<PCMTYPE>& realInput, std::vector<PCMTYPE>& realOutput)
{
    ExecuteR2C(realInput, convolutionBuffer_);

    float gain = 1.0f / GetFFTSize();

    for (size_t i = 0; i < convolutionBuffer_.size(); i++)
    {
        auto a = convolutionBuffer_[i].real();
        auto b = convolutionBuffer_[i].imag();
        auto c = convolutionKernel_[i].real();
        auto d = convolutionKernel_[i].imag();

        convolutionBuffer_[i] = std::complex<PCMTYPE>((a * c - b * d) * gain, (a * d + b * c) * gain);
    }

    ExecuteC2R(convolutionBuffer_, realOutput);
}

} // namespace FFT
} // namespace ADSSS
