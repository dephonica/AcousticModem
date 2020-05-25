#include <getopt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "Encoder.h"
#include "Payload.h"
#include "ReferenceSource.h"

static __attribute__((noreturn)) void printUsageAndExit(const char* argv0)
{
    std::cerr << "Usage: " << argv0 << " -r REFERENCE [ -i INPUT ] [ -o OUTPUT ]\n";
    exit(EXIT_FAILURE);
}

static ADSSS::PcmHelpers::ReferenceSource makeReferenceSource(const char* file)
{
    FILE* in = fopen(file, "rb");
    if (!in) {
        std::cerr << "Failed to open reference file: " << strerror(errno) << std::endl;
        return ADSSS::PcmHelpers::ReferenceSource();
    }

    std::vector<PCMTYPE> referenceData;
    size_t ret = 0;

    while (!feof(in)) {
        referenceData.resize(referenceData.size() + 1024);
        PCMTYPE* buf = referenceData.data() + referenceData.size() - 1024;
        ret = fread(buf, sizeof(PCMTYPE), 1024, in);
        if (ret < 1024) {
            if (ferror(in)) {
                std::cerr << "Failed to read reference file: " << strerror(errno) << std::endl;
                fclose(in);
                return ADSSS::PcmHelpers::ReferenceSource();
            }
            referenceData.resize(referenceData.size() - 1024 + ret);
        }
    }

    fclose(in);

    return ADSSS::PcmHelpers::ReferenceSource(ADSSS::Config::ExpectedReferenceSamplerateHz, std::move(referenceData));
}

static void encodePayload(const char* reference, const char* payload, size_t plen, FILE* out)
{
    ADSSS::PcmHelpers::ReferenceSource referenceSource = makeReferenceSource(reference);
    if (referenceSource.GetReferenceData().empty()) {
        return;
    }

    ADSSS::Payload rs;
    ADSSS::Encoder encoder(referenceSource);

    auto rsPayload = rs.Serialize(
        std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(payload), reinterpret_cast<const uint8_t*>(payload) + plen));

    if (rsPayload.size() > referenceSource.GetCapacityBytes()) {
        std::cerr << "RS payload is too big: " << rsPayload.size() << " > " << referenceSource.GetCapacityBytes() << std::endl;
        return;
    }

    auto encPayload = encoder.Go(rsPayload);

    fwrite(encPayload.data(), sizeof(encPayload[0]), encPayload.size(), out);
    if (ferror(out)) {
        std::cerr << "Failed to write encoded payload: " << strerror(errno) << std::endl;
    }
}

static std::string readPayload(FILE* fin)
{
    std::string data;

    char buf[1024];
    size_t ret = 0;
    while ((ret = fread(buf, 1, sizeof(buf), fin)) > 0) {
        data.append(buf, ret);
    }

    return data;
}

int main(int argc, char** argv)
{
    const char* config = nullptr;
    const char* reference = nullptr;
    const char* input = nullptr;
    const char* output = nullptr;
    int opt;

    FILE* fin = stdin;
    FILE* fout = stdout;

    while ((opt = getopt(argc, argv, "c:r:i:o:")) != -1) {
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
        default:
            printUsageAndExit(argv[0]);
        }
    }

    if (!reference) {
        printUsageAndExit(argv[0]);
    }

    if (config) {
        ADSSS::Config::ParseConfigFile(config);
    }

    if (input) {
        fin = fopen(input, "rb");
        if (!fin) {
            std::cerr << "Failed to open input file: " << strerror(errno) << "\n";
            return 1;
        }
    }

    if (output) {
        fout = fopen(output, "wb");
        if (!fout) {
            std::cerr << "Failed to open output file: " << strerror(errno) << "\n";
            return 1;
        }
    }

    std::string data;

    data = readPayload(fin);

    if (data.empty()) {
        std::cerr << "No payload!\n";
        return 1;
    }

    encodePayload(reference, data.c_str(), data.size(), fout);

    if (input) {
        fclose(fin);
    }

    if (output) {
        fclose(fout);
    }

    return 0;
}
