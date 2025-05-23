#include <cstring>
#include <iostream>

#include "tosunapi/tosunapi.h"

using namespace Tosun;

void ReceiveCANMsg(uint32_t canId, const std::vector<uint8_t>& data) {
    std::cout << "rx frame with id: " << canId << std::endl;
}

int main(int argc, char** argv) {
    auto can = Tosun::TosunApi::LoadDevice(ReceiveCANMsg);
    if (can.has_value()) {
        auto device = can.value();
        device.SendSync(0x11, {1, 2, 3, 4, 5, 6, 7, 8}, 1000);
    }

    return 0;
}
