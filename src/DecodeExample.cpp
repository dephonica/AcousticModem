#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <vector>

#include "Decoder.h"
#include "Payload.h"
#include "ReferenceSource.h"
#include "StreamDecoder.h"

static __attribute__((noreturn)) void printUsageAndExit(const char* argv0)
{
    std::cerr << "Usage: " << argv0 << " -r REFERENCE [ -i INPUT ] [ -o OUTPUT ]\n";
    exit(EXIT_FAILURE);
}

static int terminated = 0;

static double SteadyTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() / 1000.0;
}

static void ExitSignalHandler(int signum)
{
    std::cerr << strsignal(signum) << " [" << signum << "]\n";
    if (++terminated > 3) {
        exit(EXIT_FAILURE);
    }
}

static void InitSignalHandler()
{
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_restorer = nullptr;
    sigemptyset(&action.sa_mask);

    action.sa_handler = &ExitSignalHandler;

    sigaction(SIGTERM, &action, nullptr);
    sigaction(SIGINT, &action, nullptr);

    action.sa_handler = SIG_IGN;
    sigaction(SIGHUP, &action, nullptr);
}

class PayloadWriter : public ADSSS::IStreamCallbacks
{
public:
    explicit PayloadWriter(FILE* out)
        : fout_(out)
    {
    }

    void OnCorrelatorTriggered(const std::vector<PCMTYPE>& /*samplesData*/, size_t /*samplesCount*/) override
    {
        correlationTriggerTime_ = SteadyTime();
        std::cerr << "Correlation triggered at " << correlationTriggerTime_ << std::endl;
    }

    void OnDataDecoded(const std::vector<uint8_t>& serializedData) override
    {
        std::cerr << __func__ << std::endl;
        double now = SteadyTime();
        std::cerr << "Got payload " << serializedData.size() << " bytes long at " << now << "\n";
        std::cerr << "Data has been decoded within " << now - correlationTriggerTime_ << " seconds\n";

        ADSSS::Payload rs;
        auto payload = rs.Deserialize(serializedData);

        if (rs.GetErrorsCorrected() < 0) {
            std::cerr << "Failed to deserialize payload\n";
        }
        else {
            std::cerr << "Errors corrected by RS code: " << rs.GetErrorsCorrected() << std::endl;
        }

        fwrite(payload.data(), payload.size(), 1, fout_);

        if (ferror(fout_)) {
            std::cerr << "write failed: " << strerror(errno) << std::endl;
        }
        else {
            fflush(fout_);
        }
    }

private:
    FILE* fout_;
    double correlationTriggerTime_;
};

template<typename T>
std::vector<T> FileToVector(FILE* in)
{
    std::vector<T> data;
    size_t ret = 0;
    while (!feof(in)) {
        data.resize(data.size() + 1024);
        T* buf = data.data() + data.size() - 1024;
        ret = fread(buf, sizeof(T), 1024, in);

        if (ret < 1024) {
            if (ferror(in)) {
                std::cerr << "Failed to read data from file: " << strerror(errno) << std::endl;
                data.clear();
                break;
            }
            data.resize(data.size() - 1024 + ret);
        }
    }

    return data;
}

static ADSSS::PcmHelpers::ReferenceSource makeReferenceSource(const char* file)
{
    FILE* in = fopen(file, "rb");
    if (!in) {
        std::cerr << "Failed to open reference file: " << strerror(errno) << std::endl;
        return ADSSS::PcmHelpers::ReferenceSource();
    }

    std::vector<PCMTYPE> referenceData = FileToVector<PCMTYPE>(in);

    fclose(in);

    if (referenceData.empty()) {
        std::cerr << "Failed to read reference file" << std::endl;
        return ADSSS::PcmHelpers::ReferenceSource();
    }

    return ADSSS::PcmHelpers::ReferenceSource(ADSSS::Config::ExpectedReferenceSamplerateHz, std::move(referenceData));
}

static void decodeStreamPayload(const char* reference, FILE* in, FILE* out)
{
    ADSSS::PcmHelpers::ReferenceSource referenceSource = makeReferenceSource(reference);
    if (referenceSource.GetReferenceData().empty()) {
        return;
    }

    PayloadWriter payloadWriter(out);
    ADSSS::StreamDecoder decoder(referenceSource, payloadWriter);

    size_t sampleSize = sizeof(PCMTYPE);
    std::vector<PCMTYPE> buffer(ADSSS::Config::ExpectedReferenceSamplerateHz / 2);

    size_t totalSamples = 0;
    bool carrierDetected = false;

    while (!feof(in) && !terminated) {
        size_t ret = fread(buffer.data(), sampleSize, buffer.size(), in);

        if (terminated) {
            return;
        }

        if (ret == 0) {
            if (ferror(in)) {
                std::cerr << "Failed to read input stream: " << strerror(errno) << std::endl;
            }
            break;
        }

        size_t samplesRead = ret;
        totalSamples += samplesRead;

        bool detected = decoder.PushSamples(buffer, 0, samplesRead);
        if (detected != carrierDetected) {
            std::cerr << "Carrier " << (detected ? "detected\n" : "lost\n");
            carrierDetected = detected;
        }
    }

    std::cerr << "samples processed: " << totalSamples << "\n";
}

static void decodeStaticPayload(const char* reference, FILE* in, FILE* out)
{
    auto payload = FileToVector<PCMTYPE>(in);
    if (payload.empty()) {
        std::cerr << "Failed to read payload\n";
        return;
    }

    ADSSS::PcmHelpers::ReferenceSource referenceSource = makeReferenceSource(reference);
    if (referenceSource.GetReferenceData().empty()) {
        return;
    }

    ADSSS::PcmHelpers::ReferenceSource encodedSource(ADSSS::Config::ExpectedReferenceSamplerateHz, std::move(payload));
    ADSSS::Decoder decoder(encodedSource, referenceSource);

    PayloadWriter payloadWriter(out);
    payloadWriter.OnDataDecoded(decoder.Go());
}

int main(int argc, char** argv)
{
    InitSignalHandler();

    const char* config = nullptr;
    const char* reference = nullptr;
    const char* input = nullptr;
    const char* output = nullptr;
    bool stream = false;

    FILE* fin = stdin;
    FILE* fout = stdout;

    int opt;

    while ((opt = getopt(argc, argv, "c:r:i:o:s")) != -1) {
        switch (opt) {
        case 'c':
            config = optarg;
            break;
        case 'r':
            reference = optarg;
            break;
        case 'i':
            input = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case 's':
            stream = true;
            break;
        default:
            printUsageAndExit(argv[0]);
        }
    }

    if (stream && !reference) {
        printUsageAndExit(argv[0]);
    }

    if (config) {
        ADSSS::Config::ParseConfigFile(config);
    }

    if (input) {
        fin = fopen(input, "rb");
        if (!fin) {
            std::cerr << "Failed to open input file: " << strerror(errno) << std::endl;
            return 1;
        }
    }

    if (output) {
        fout = fopen(output, "wb");
        if (!fout) {
            std::cerr << "Failed to open output file: " << strerror(errno) << std::endl;
            return 1;
        }
    }

    if (stream) {
        decodeStreamPayload(reference, fin, fout);
    }
    else {
        decodeStaticPayload(reference, fin, fout);
    }

    if (input) {
        fclose(fin);
    }

    if (output) {
        fclose(fout);
    }

    return 0;
}
