//
// Created by quinn on 5/26/2025.
// Copyright (c) 2025 All rights reserved.
//

#ifndef CAN_MSG_H
#define CAN_MSG_H

#include <cstdint>
#include <vector>

namespace Tosun {

class CanMsg {
public:
    CanMsg(uint32_t id, const uint8_t* data, const uint8_t len, bool isCANFD = false) :
        id_{id},
        data_{data, data + len},
        isCANFD_(isCANFD) {}

    uint32_t Id() const { return id_; }
    const std::vector<uint8_t>& Data() const { return data_; }
    bool IsCANFD() const { return isCANFD_; }

private:
    uint32_t id_;
    std::vector<uint8_t> data_;
    bool isCANFD_;
};

}

#endif //CAN_MSG_H
