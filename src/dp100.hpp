#ifndef DP100_HPP
#define DP100_HPP

#include <string>
#include "hidapi/hidapi.h"
#include <vector>

#define VID 0x2e3c
#define PID 0xaf01

#define DR_H2D 0xFB
#define DR_D2H 0xFA

#define OP_NONE 0x00
#define OP_DEVICEINFO 0x10
#define OP_BASICINFO 0x30
#define OP_BASICSET 0x35
#define OP_SYSTEMINFO 0x40
#define OP_SCANOUT 0x50
#define OP_SERIALOUT 0x55
#define SET_MODIFY 0x20
#define SET_ACT 0x80

class DP100 {
public:
    DP100();
    ~DP100();

    bool connect();
    void disconnect();
    void setVoltage(double voltage);
    void setCurrent(double current);
    void enable();
    void disable();
    void getStatus();

private:
    hid_device* device; // HID device handle
    double currentVoltage;
    double currentCurrent;
    bool isEnabled;
    std::vector<uint8_t> generateFrame(uint8_t opCode, std::vector<uint8_t> data = {});
    std::vector<uint8_t> generateSet(bool output=false, uint16_t voltage_set =0, uint16_t current_set=0, uint16_t overvoltage = 30500, uint16_t overcurrent = 30500);
    void sendCommand(uint8_t* command, size_t length);
    void readResponse();
    uint16_t crc16(uint8_t* data, size_t length);
};

#endif // DP100_HPP