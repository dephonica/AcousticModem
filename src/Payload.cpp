#include "Payload.h"

#include <assert.h>
#include <string.h>

#include <arpa/inet.h> // htons(), ntohs()

namespace ADSSS {

std::vector<uint8_t> Payload::Serialize(const std::vector<uint8_t>& rawPayload)
{
    size_t nRsBlocks = (rawPayload.size() + 2 + Config::ReedSolomonPayloadSize - 1) / Config::ReedSolomonPayloadSize;
    std::vector<uint8_t> rsBlock;
    rsBlock.reserve((Config::ReedSolomonBlockSize));
    std::vector<uint8_t> rsPayload;
    rsPayload.reserve(nRsBlocks * Config::ReedSolomonBlockSize);
    size_t offset = 0;

    for (size_t n = 0; n < nRsBlocks; ++n) {
        rsBlock.resize(Config::ReedSolomonPayloadSize);
        if (!n) {
            uint16_t payloadSize = htons(static_cast<uint16_t>(rawPayload.size()));
            memcpy(rsBlock.data(), &payloadSize, 2);
            size_t bytesToCopy = std::min<size_t>(rawPayload.size(), Config::ReedSolomonPayloadSize - 2);
            memcpy(rsBlock.data() + 2, rawPayload.data(), bytesToCopy);
            offset = bytesToCopy;
        }
        else {
            size_t bytesToCopy = std::min<size_t>(rawPayload.size() - offset, Config::ReedSolomonPayloadSize);
            memcpy(rsBlock.data(), rawPayload.data() + offset, bytesToCopy);
            offset += bytesToCopy;
        }

        reedSolomon_.encode(rsBlock);
        rsPayload.insert(rsPayload.end(), rsBlock.begin(), rsBlock.end());
    }

    return rsPayload;
}

std::vector<uint8_t> Payload::Deserialize(const std::vector<uint8_t>& serializedData)
{
    if (serializedData.size() < Config::ReedSolomonBlockSize) {
        errorsCorrected_ = -1;
        return std::vector<uint8_t>();
    }

    errorsCorrected_ = 0;
    size_t nRsBlocks = serializedData.size() / Config::ReedSolomonBlockSize;
    std::vector<uint8_t> rsBlock(Config::ReedSolomonBlockSize);
    std::vector<uint8_t> payload;
    size_t offset = 0;

    for (size_t n = 0; n < nRsBlocks; ++n) {
        const uint8_t* block = serializedData.data() + n * Config::ReedSolomonBlockSize;
        memcpy(rsBlock.data(), block, Config::ReedSolomonBlockSize);

        int err = reedSolomon_.decode(rsBlock);
        if (err < 0) {
            errorsCorrected_ = -1;
            payload.clear();
            break;
        }

        errorsCorrected_ += err;

        if (!n) {
            size_t payloadSize = ntohs(*reinterpret_cast<uint16_t*>(rsBlock.data()));
            size_t maxPayloadSize = nRsBlocks * Config::ReedSolomonPayloadSize - 2;
            if (payloadSize > maxPayloadSize) {
                std::cerr << "Invalid payload size: " << payloadSize << std::endl;
                errorsCorrected_ = -1;
                break;
            }

            payload.resize(payloadSize);
            size_t bytesToCopy = std::min<size_t>(payloadSize, Config::ReedSolomonPayloadSize - 2);
            memcpy(payload.data(), rsBlock.data() + 2, bytesToCopy);
            offset = bytesToCopy;
        }
        else {
            size_t bytesToCopy = std::min<size_t>(Config::ReedSolomonPayloadSize, payload.size() - offset);
            memcpy(payload.data() + offset, rsBlock.data(), bytesToCopy);
            offset += bytesToCopy;
        }

        if (offset == payload.size()) {
            break;
        }
    }

    return payload;
}

} // namespace ADSSS
