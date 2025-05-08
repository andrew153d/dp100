#include "dp100.hpp"
#include <iostream>
#include <hidapi/hidapi.h>
#include <cstring>

DP100::DP100() : device(nullptr) {
}

DP100::~DP100() {
    disconnect();
}

bool DP100::connect() {
    device = hid_open(VID, PID, nullptr);
    return device != nullptr;
}

void DP100::disconnect() {
    if (device) {
        hid_close(device);
        device = nullptr;
    }
}

void DP100::setVoltage(double voltage) {
    uint8_t command[8] = {0x20, static_cast<uint8_t>(voltage * 1000) & 0xFF, (static_cast<uint8_t>(voltage * 1000) >> 8) & 0xFF};
    sendCommand(command, sizeof(command));
}

void DP100::setCurrent(double current) {
    uint8_t command[8] = {0x21, static_cast<uint8_t>(current * 1000) & 0xFF, (static_cast<uint8_t>(current * 1000) >> 8) & 0xFF};
    sendCommand(command, sizeof(command));
}

void DP100::enable() {
    uint8_t command[1] = {0x80};
    sendCommand(command, sizeof(command));
}

void DP100::disable() {
    uint8_t command[1] = {0x00};
    sendCommand(command, sizeof(command));
}

void DP100::getStatus() {
    uint8_t command[1] = {0x40};
    sendCommand(command, sizeof(command));
    readResponse();
}

std::vector<uint8_t> DP100::generateFrame(uint8_t opCode, std::vector<uint8_t> data)
{
    std::vector<uint8_t> frame;
    frame.push_back(0xFB); // DR_H2D
    frame.push_back(opCode & 0xFF);
    frame.push_back(0x00);
    frame.push_back(static_cast<uint8_t>(data.size() & 0xFF));
    frame.insert(frame.end(), data.begin(), data.end());

    uint16_t crc = crc16(frame.data(), frame.size());
    frame.push_back(static_cast<uint8_t>(crc & 0xFF));
    frame.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));

    return frame;
}

std::vector<uint8_t> DP100::generateSet(bool output=false, uint16_t voltage_set =0, uint16_t current_set=0, uint16_t overvoltage = 30500, uint16_t overcurrent = 30500)
{
    std::vector<uint8_t> setData;
    setData.push_back(SET_MODIFY);
    setData.push_back(output ? 1 : 0);
    setData.push_back(voltage_set & 0xFF);
    setData.push_back((voltage_set >> 8) & 0xFF);
    setData.push_back(current_set & 0xFF);
    setData.push_back((current_set >> 8) & 0xFF);
    setData.push_back(overvoltage & 0xFF);
    setData.push_back((overvoltage >> 8) & 0xFF);
    setData.push_back(overcurrent & 0xFF);
    setData.push_back((overcurrent >> 8) & 0xFF);
    return setData;
}

void DP100::sendCommand(uint8_t* command, size_t length) {
    if (device) {
        printf("Sending command: ");
        for (size_t i = 0; i < length; i++) {
            printf("%02X ", command[i]);
        }
        printf("\n");
        hid_write(device, command, length);
    }else {
        std::cerr << "Device not connected.\n";
    }
}

void DP100::readResponse() {
    uint8_t response[64];
    int res = hid_read(device, response, sizeof(response));
    if (res > 0) {
        // Process response
    }
}

uint16_t DP100::crc16(uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}