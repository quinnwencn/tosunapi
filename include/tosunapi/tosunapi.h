//
// Created by quinn on 5/23/2025.
// Copyright (c) 2025 All rights reserved.
//

#ifndef TOSUNAPI_H
#define TOSUNAPI_H

#include <cstdint>
#include <optional>
#include <functional>
#include <vector>

namespace Tosun {

enum class TosunResult {
    OK = 0,
    ERROR = 1,
    INVALID_PARAM = 2,
    INVALID_LENGTH = 3,
    CONNECTED = 5,
};

class TosunApi {
public:
    ~TosunApi();
    using Callback = std::function<void(uint32_t canId, const std::vector<uint8_t>& data)>;

    static std::optional<TosunApi> LoadDevice(Callback canCb, Callback canFdCb = nullptr);

    TosunResult SendSync(uint32_t canId, const std::vector<uint8_t>& data, uint32_t timeout) const;
    TosunResult SendAsync(uint32_t canId, const std::vector<uint8_t>& data) const;

    uint64_t DeviceId() const { return deviceId_; }

private:
    explicit TosunApi(uint64_t id, Callback canCb) : deviceId_(id), canCb_(canCb) {}
    TosunResult Send(uint32_t canId, const std::vector<uint8_t>& data, uint32_t timeout, bool sync=true) const;

    uint64_t deviceId_;
    Callback canCb_;
};

}

#endif //TOSUNAPI_H
