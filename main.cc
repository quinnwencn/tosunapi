#include <iostream>

#include "tosunapi/tosunapi.h"

using namespace Tosun;

void ReceiveCANMsg(uint32_t canId, const uint8_t* data, uint8_t len) {
    std::cout << "rx frame with id: " << canId << std::endl;
}

int main(int argc, char** argv) {
    TosunApi device{ReceiveCANMsg};
    auto res = device.ConnectDevice();
    if (res != TosunResult::OK) {
        std::cout << "connect failed" << std::endl;
    }

    res = device.SendSync(0x11, {1, 2, 3, 4, 5, 6, 7, 8}, 500);
    if (res != TosunResult::OK) {
        std::cout << "sendsync failed" << std::endl;
    }

    device.DisconnectDevice();
    return 0;
}
