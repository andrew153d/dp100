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
    void checkFrame(const std::vector<uint8_t>& data);

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
    void readStatus();



    // Variables to hold data read in checkFrame
    uint16_t vin, vout, iout, vo_max, temp1, temp2, dc_5v;
    uint8_t out_mode, work_st;
    std::string dev_type;
    uint16_t hdw_ver, app_ver, boot_ver, run_area;
    std::vector<uint8_t> dev_sn;
    uint16_t year;
    uint8_t month, day;
    uint8_t blk_lev;
    uint16_t opp, opt;
    uint8_t vol_lev;
    uint8_t index, state;
    uint16_t vo_set, io_set, ovp_set, ocp_set;
};

#endif // DP100_HPP