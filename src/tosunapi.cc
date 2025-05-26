//
// Created by quinn on 5/23/2025.
// Copyright (c) 2025 All rights reserved.
//
#include "tosunapi/tosunapi.h"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <TSCANDef.h>

#include "tosunapi/thread_safe_queue.h"
#include "tosunapi/can_msg.h"

namespace {

constexpr uint8_t MASK_DIR_TX = 0x1;
Tosun::ThreadSafeQue<std::unique_ptr<Tosun::CanMsg>, 100> threadSafeQue;

} // namespace

namespace Tosun {

void LibTscanWrapper::ConnectDevice() {
    int expected = 0;
    if (deviceCount_.compare_exchange_strong(expected, 1,
            std::memory_order_acq_rel,
            std::memory_order_acquire)) {
        initialize_lib_tscan(true, false, false);
    } else {
        deviceCount_.fetch_add(1, std::memory_order_acq_rel);
    }
}

void LibTscanWrapper::DisconnectDevice() {
    int expected = 1;
    if (deviceCount_.compare_exchange_strong(expected, 0,
            std::memory_order_acq_rel,
            std::memory_order_acquire)) {
        finalize_lib_tscan();
    } else {
        deviceCount_.fetch_sub(1, std::memory_order_acq_rel);
    }
}

TosunApi::TosunApi(Callback cb) : cb_(cb), deviceId_(UINT64_MAX) {
    running_ = true;
    auto process = [&]() {
        while (running_) {
            auto msg = threadSafeQue.Pop();
            if (cb_ != nullptr) {
                auto& data = msg->Data();
                cb_(msg->Id(),data.data(), data.size());
            }
        }
    };

    msgProcessor_ = std::thread(process);
}

TosunApi::~TosunApi() {
    running_ = false;
    msgProcessor_.join();
}

TosunResult TosunApi::ConnectDevice() {
    LibTscanWrapper::Instance().ConnectDevice();
    uint32_t deviceNum = 0;
    auto ret = tscan_scan_devices(&deviceNum);
    if (static_cast<TosunResult>(ret) != TosunResult::OK || deviceNum == 0) {
        return TosunResult::ERROR;
    }

    char* manufacturer = nullptr;
    char* product = nullptr;
    char* serial = nullptr;
    ret = tscan_get_device_info(0, &manufacturer, &product, &serial);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        return TosunResult::ERROR;
    }

    uint64_t deviceId = 0;
    ret = tscan_connect(serial, &deviceId);
    if (static_cast<TosunResult>(ret) != TosunResult::OK && ret != static_cast<uint32_t>(TosunResult::CONNECTED)) {
        return TosunResult::FAILED_TO_CONNECT;
    }

    auto canCallback = [](const TLibCAN *msg) {
        if (!(msg->FProperties & MASK_DIR_TX)) {
            auto canId = msg->FIdentifier;
            const uint8_t *data = msg->FData;
            uint8_t len = msg->FDLC;

            auto canMsg = std::make_unique<CanMsg>(canId, data, len);
            threadSafeQue.Push(std::move(canMsg));
        }
    };

    ret = tscan_register_event_can(deviceId, canCallback);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        tscan_disconnect_by_handle(deviceId);
        return TosunResult::REGISTER_CAN_CALLBACK_FAIL;
    }

    auto canfdCallback = [](const TLibCANFD* msg) {
        if (!(msg->FProperties & MASK_DIR_TX)) {
            auto canId = msg->FIdentifier;
            const uint8_t *data = msg->FData;
            uint8_t len = msg->FDLC;
            auto canMsg = std::make_unique<CanMsg>(canId, data, len, true);
            threadSafeQue.Push(std::move(canMsg));
        }
    };
    ret = tscan_register_event_canfd(deviceId, canfdCallback);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        tscan_disconnect_by_handle(deviceId);
        return TosunResult::REGISTER_CANFD_CALLBACK_FAIL;
    }

    ret = tscan_configure_canfd_regs(deviceId, static_cast<APP_CHANNEL>(CHN1), 500, 63, 16, 1, 15, 2000,
          15, 4, 1, 3, lfdtISOCAN, lfdmNormal, 1, 0);
    ret |= tscan_configure_canfd_regs(deviceId, static_cast<APP_CHANNEL>(CHN2), 500, 63, 16, 1, 15, 2000,
              15, 4, 1, 3, lfdtISOCAN, lfdmNormal, 1, 0);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        tscan_disconnect_by_handle(deviceId);
        return TosunResult::CONFIGURE_FAIL;
    }

    deviceId_ = deviceId;
    return TosunResult::OK;
}

void TosunApi::DisconnectDevice() {
    LibTscanWrapper::Instance().DisconnectDevice();
}

TosunResult TosunApi::SendSync(uint32_t canId, const std::vector<uint8_t>& data, uint32_t timeout) const {
    return Send(canId, data, timeout, true);
}

TosunResult TosunApi::SendAsync(uint32_t canId, const std::vector<uint8_t>& data) const {
    return Send(canId, data, 0, false);
}

TosunResult TosunApi::Send(uint32_t canId, const std::vector<uint8_t>& data, uint32_t timeout, bool sync) const {
    if (data.size() == 0 || data.size() > 8) {
        return TosunResult::INVALID_LENGTH;
    }

    TLibCAN canMsg {};
    canMsg.FIdentifier = canId;
    canMsg.FProperties = 0x00; // standard frame
    canMsg.FDLC = data.size();
    canMsg.FIdxChn = CHN1;
    memcpy(canMsg.FData, data.data(), data.size());

    uint32_t ret = 0;
    if (sync == false) {
        ret = tscan_transmit_can_async(deviceId_, &canMsg);
    } else {
        ret = tscan_transmit_can_sync(deviceId_, &canMsg, timeout);
    }

    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        return TosunResult::ERROR;
    }

    return TosunResult::OK;
}

}
