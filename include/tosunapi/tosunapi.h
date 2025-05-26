//
// Created by quinn on 5/23/2025.
// Copyright (c) 2025 All rights reserved.
//

#ifndef TOSUNAPI_H
#define TOSUNAPI_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <vector>
#include <thread>

namespace Tosun {

enum class TosunResult {
    OK = 0,
    ERROR = 1,
    INVALID_PARAM = 2,
    INVALID_LENGTH = 3,
    CONNECTED = 5,
    FAILED_TO_CONNECT,
    REGISTER_CAN_CALLBACK_FAIL,
    REGISTER_CANFD_CALLBACK_FAIL,
    CONFIGURE_FAIL
};

class LibTscanWrapper {
public:
    static LibTscanWrapper& Instance() {
        static LibTscanWrapper instance;
        return instance;
    }

    void ConnectDevice();
    void DisconnectDevice();

    LibTscanWrapper(const LibTscanWrapper&) = delete;
    LibTscanWrapper& operator=(const LibTscanWrapper&) = delete;
private:
    LibTscanWrapper() : deviceCount_(0) {}
    ~LibTscanWrapper() {
        if (deviceCount_.load() > 0) {
            // Forget to disconnect device.
        }
    }

    std::atomic<int> deviceCount_;
};

class TosunApi {
public:
    using Callback = std::function<void(uint32_t canId, const uint8_t* data, uint32_t len)>;

    TosunApi(Callback cb);
    ~TosunApi();

    TosunResult ConnectDevice();
    void DisconnectDevice();
    TosunResult SendSync(uint32_t canId, const std::vector<uint8_t>& data, uint32_t timeout) const;
    TosunResult SendAsync(uint32_t canId, const std::vector<uint8_t>& data) const;

    uint64_t DeviceId() const { return deviceId_; }

private:
    TosunResult Send(uint32_t canId, const std::vector<uint8_t>& data, uint32_t timeout, bool sync=true) const;

    Callback cb_;
    uint64_t deviceId_;
    bool running_;
    std::thread msgProcessor_;
};

}

#endif //TOSUNAPI_H
