//
// Created by quinn on 5/23/2025.
// Copyright (c) 2025 All rights reserved.
//
#include "tosunapi/tosunapi.h"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <TSCANDef.h>

void ReceiveCANMessage(const TLibCAN *AData)
{
    if ((AData->FProperties & 0x1) == 1)
    {
        std::cout << "tx frame with id" << AData->FIdentifier << " on " << AData->FIdxChn << std::endl;
    }
    else
    {
        std::cout << "rx frame with id" << AData->FIdentifier << " on " << AData->FIdxChn << std::endl;
    }
}

void ReceiveCANFDMessage(const TLibCANFD *AFDData)
{
    if ((AFDData->FProperties & 0x1) == 1)
    {
        std::cout << "tx CAN FD frame with id" << AFDData->FIdentifier << std::endl;
    }
    else
    {
        std::cout << "rx CAN FD frame with id " << AFDData->FIdentifier << std::endl;
    }
}

namespace Tosun {

TosunApi::~TosunApi() {
    // finalize_lib_tscan();
}

std::optional<TosunApi> TosunApi::LoadDevice(Callback canCb, Callback canFdCb) {
    initialize_lib_tscan(true, false, false);
    uint32_t deviceNum = 0;
    auto ret = tscan_scan_devices(&deviceNum);
    if (static_cast<TosunResult>(ret) != TosunResult::OK || deviceNum == 0) {
        return std::nullopt;
    }

    char* manufacturer = nullptr;
    char* product = nullptr;
    char* serial = nullptr;
    ret = tscan_get_device_info(0, &manufacturer, &product, &serial);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        return std::nullopt;
    }

    uint64_t deviceId = 0;
    ret = tscan_connect(serial, &deviceId);
    if (static_cast<TosunResult>(ret) != TosunResult::OK && ret != static_cast<uint32_t>(TosunResult::CONNECTED)) {
        return std::nullopt;
    }

    ret = tscan_register_event_can(deviceId, ReceiveCANMessage);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        tscan_disconnect_by_handle(deviceId);
        return std::nullopt;
    }

    ret = tscan_register_event_canfd(deviceId, ReceiveCANFDMessage);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        tscan_disconnect_by_handle(deviceId);
        return std::nullopt;
    }

    ret = tscan_configure_canfd_regs(deviceId, static_cast<APP_CHANNEL>(CHN1), 500, 63, 16, 1, 15, 2000,
          15, 4, 1, 3, lfdtISOCAN, lfdmNormal, 1, 0);
    ret |= tscan_configure_canfd_regs(deviceId, static_cast<APP_CHANNEL>(CHN2), 500, 63, 16, 1, 15, 2000,
              15, 4, 1, 3, lfdtISOCAN, lfdmNormal, 1, 0);
    if (static_cast<TosunResult>(ret) != TosunResult::OK) {
        tscan_disconnect_by_handle(deviceId);
        return std::nullopt;
    }

    if (canFdCb != nullptr) {
        throw std::runtime_error("CAN FD is not supported yet.");
    }

    return TosunApi(deviceId, canCb);
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
