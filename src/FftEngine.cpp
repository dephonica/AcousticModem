#include "FftEngine.h"

#include <assert.h>

namespace ADSSS {
namespace FFT {

FftEngine::FftEngine(size_t fftSize)
    : kissBufferCpx_(fftSize / 2 + 1)
    , kissBufferScalar_(fftSize)
    , convolutionBuffer_(fftSize / 2 + 1)
    , convolutionKernel_(fftSize)
{
    forwardFft_ = kiss_fftr_alloc(static_cast<int>(fftSize), false, nullptr, nullptr);
    inverseFft_ = kiss_fftr_alloc(static_cast<int>(fftSize), true, nullptr, nullptr);
}

FftEngine::~FftEngine()
{
    kiss_fft_free(inverseFft_);
    kiss_fft_free(forwardFft_);
}

static void copyToKissBuffer(const std::vector<PCMTYPE>& in, std::vector<kiss_fft_scalar>& out)
{
    size_t n = 0;
    while (n < std::min(in.size(), out.size())) {
        out[n] = in[n];
        ++n;
    }

    while (n < out.size()) {
        out[n] = 0;
        ++n;
    }
}

static void copyToKissBuffer(const std::vector<std::complex<PCMTYPE>>& in, std::vector<kiss_fft_cpx>& out)
{
    size_t n = 0;
    while (n < std::min(in.size(), out.size())) {
        out[n].r = in[n].real();
        out[n].i = in[n].imag();
        ++n;
    }

    while (n < out.size()) {
        out[n].r = 0;
        out[n].i = 0;
        ++n;
    }
}

static void copyFromKissBuffer(const std::vector<kiss_fft_scalar>& in, std::vector<PCMTYPE>& out)
{
    size_t n = 0;
    while (n < std::min(in.size(), out.size())) {
        out[n] = in[n];
        ++n;
    }

    while (n < out.size()) {
        out[n] = 0;
        ++n;
    }
}

static void copyFromKissBuffer(const std::vector<kiss_fft_cpx>& in, std::vector<std::complex<PCMTYPE>>& out)
{
    size_t n = 0;
    while (n < std::min(in.size(), out.size())) {
        out[n].real(in[n].r);
        out[n].imag(in[n].i);
        ++n;
    }

    while (n < out.size()) {
        out[n].real(0);
        out[n].imag(0);
        ++n;
    }
}

void FftEngine::ExecuteR2C(const std::vector<PCMTYPE>& realBuffer, std::vector<std::complex<PCMTYPE>>& complexBuffer)
{
    copyToKissBuffer(realBuffer, kissBufferScalar_);
    kiss_fftr(forwardFft_, kissBufferScalar_.data(), kissBufferCpx_.data());
    copyFromKissBuffer(kissBufferCpx_, complexBuffer);
}

void FftEngine::ExecuteC2R(const std::vector<std::complex<PCMTYPE>>& complexBuffer, std::vector<PCMTYPE>& realBuffer)
{
    copyToKissBuffer(complexBuffer, kissBufferCpx_);
    kiss_fftri(inverseFft_, kissBufferCpx_.data(), kissBufferScalar_.data());
    copyFromKissBuffer(kissBufferScalar_, realBuffer);
}

void FftEngine::SetConvolutionKernel(const std::vector<std::complex<PCMTYPE>>& complexKernel)
{
    convolutionKernel_.assign(complexKernel.begin(), complexKernel.end());
}

void FftEngine::ExecuteConvolution(const std::vector<PCMTYPE>& realInput, std::vector<PCMTYPE>& realOutput)
{
    ExecuteR2C(realInput, convolutionBuffer_);

    float gain = 1.0f / GetFFTSize();

    for (size_t i = 0; i < convolutionBuffer_.size(); i++) {
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
