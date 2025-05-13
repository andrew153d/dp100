#include "dp100.hpp"
#include <iostream>
#include <hidapi/hidapi.h>
#include <cstring>
#include <thread>
#include <fstream>
#include <sstream>

DP100::DP100() : device(nullptr), currentVoltage(0.0), currentCurrent(0.0), isEnabled(false)
{
    loadSettings();
}

DP100::~DP100()
{
    disconnect();
}

bool DP100::connect()
{
    device = hid_open(VID, PID, nullptr);
    if (device)
    {
    }
    return device != nullptr;
}

void DP100::disconnect()
{
    if (device)
    {
        hid_close(device);
        device = nullptr;
    }
}

void DP100::setVoltage(double voltage)
{
    currentVoltage = voltage * 1000; // Convert to mV
    auto command = generateFrame(OP_BASICSET, generateSet(isEnabled, currentVoltage, currentCurrent));
    hid_write(device, command.data(), command.size());
    saveSettings();
}

void DP100::setCurrent(double current)
{
    currentCurrent = current * 1000; // Convert to mA
    auto command = generateFrame(OP_BASICSET, generateSet(isEnabled, currentVoltage, currentCurrent));
    hid_write(device, command.data(), command.size());
    saveSettings();
}

void DP100::enable()
{
    isEnabled = true;
    auto command = generateFrame(OP_BASICSET, generateSet(isEnabled, currentVoltage, currentCurrent));
    hid_write(device, command.data(), command.size());
}

void DP100::disable()
{
    isEnabled = false;
    auto command = generateFrame(OP_BASICSET, generateSet(isEnabled, currentVoltage, currentCurrent));
    hid_write(device, command.data(), command.size());
}

void DP100::readStatus()
{
    {
        int status_read = 0;
        auto msg = generateFrame(OP_DEVICEINFO);
        std::vector<uint8_t> response(64);
        hid_write(device, msg.data(), msg.size());
        int res = hid_read(device, response.data(), response.size());
        if (res > 0)
        {
            checkFrame(response);
        }
    }
    // wait 100ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        int status_read = 0;
        auto msg = generateFrame(OP_BASICINFO);
        std::vector<uint8_t> response(64);
        hid_write(device, msg.data(), msg.size());
        int res = hid_read(device, response.data(), response.size());
        if (res > 0)
        {
            checkFrame(response);
        }
    }
    // wait 100ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        int status_read = 0;
        auto msg = generateFrame(OP_BASICSET, std::vector<uint8_t>{SET_ACT});
        std::vector<uint8_t> response(64);
        hid_write(device, msg.data(), msg.size());
        int res = hid_read(device, response.data(), response.size());
        if (res > 0)
        {
            checkFrame(response);
        }
    }
}

void DP100::getStatus()
{

    int tries = 0;
    while (readBasicInfo() == false && tries < 3)
    {
        tries++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (tries == 3)
    {
        printf("Failed to read basic info after 3 attempts.\n");
        return;
    }
    tries = 0;
    while (readBasicSet() == false && tries < 3)
    {
        tries++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (tries == 3)
    {
        printf("Failed to read basic set after 3 attempts.\n");
        return;
    }
    tries = 0;
    while (readDeviceInfo() == false && tries < 3)
    {
        tries++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (tries == 3)
    {
        printf("Failed to read device info after 3 attempts.\n");
        return;
    }

    printf("Vin: %u\n", vin);
    printf("Vout: %u\n", vout);
    printf("Iout: %u\n", iout);
    printf("Vo_max: %u\n", vo_max);
    printf("Temp1: %u\n", temp1);
    printf("Temp2: %u\n", temp2);
    printf("DC 5V: %u\n", dc_5v);
    printf("Output Mode: %u\n", out_mode);
    printf("Work State: %u\n", work_st);
    printf("Device Type: %s\n", dev_type.c_str());
    printf("Hardware Version: %u\n", hdw_ver);
    printf("Application Version: %u\n", app_ver);
    printf("Bootloader Version: %u\n", boot_ver);
    printf("Run Area: %u\n", run_area);
    printf("Device Serial Number: ");
    for (auto byte : dev_sn)
    {
        printf("%02X ", byte);
    }
    printf("\n");
    printf("Manufacture Date: %u-%02u-%02u\n", year, month, day);
    printf("Block Level: %u\n", blk_lev);
    printf("OPP: %u\n", opp);
    printf("OPT: %u\n", opt);
    printf("Voltage Level: %u\n", vol_lev);
    printf("Index: %u\n", index);
    printf("State: %u\n", state);

    printf("Voltage Set: %u\n", vo_set);
    printf("Current Set: %u\n", io_set);
    printf("Overvoltage Set: %u\n", ovp_set);
    printf("Overcurrent Set: %u\n", ocp_set);
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

std::vector<uint8_t> DP100::generateSet(bool output, uint16_t voltage_set, uint16_t current_set, uint16_t overvoltage, uint16_t overcurrent)
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

void DP100::checkFrame(const std::vector<uint8_t> &data)
{
    if (data[0] == DR_D2H)
    {
        uint8_t op = data[1];
        uint8_t data_len = data[3];
        if (crc16(const_cast<uint8_t *>(data.data()), 4 + data_len + 2) == 0)
        { // correct if return 0
            std::vector<uint8_t> raw_data(data.begin() + 4, data.begin() + 4 + data_len);
            if (op == OP_BASICINFO)
            {
                vin = (raw_data[1] << 8) | raw_data[0];
                vout = (raw_data[3] << 8) | raw_data[2];
                iout = (raw_data[5] << 8) | raw_data[4];
                vo_max = (raw_data[7] << 8) | raw_data[6];
                temp1 = (raw_data[9] << 8) | raw_data[8];
                temp2 = (raw_data[11] << 8) | raw_data[10];
                dc_5v = (raw_data[13] << 8) | raw_data[12];
                out_mode = raw_data[14];
                work_st = raw_data[15];
            }
            else if (op == OP_DEVICEINFO)
            {
                dev_type = std::string(reinterpret_cast<const char *>(&raw_data[0]), 15);
                hdw_ver = (raw_data[17] << 8) | raw_data[16];
                app_ver = (raw_data[19] << 8) | raw_data[18];
                boot_ver = (raw_data[21] << 8) | raw_data[20];
                run_area = (raw_data[23] << 8) | raw_data[22];
                dev_sn = std::vector<uint8_t>(raw_data.begin() + 24, raw_data.begin() + 35);
                year = (raw_data[37] << 8) | raw_data[36];
                month = raw_data[38];
                day = raw_data[39];
            }
            else if (op == OP_SYSTEMINFO)
            {
                blk_lev = raw_data[0];
                opp = (raw_data[2] << 8) | raw_data[1];
                opt = (raw_data[4] << 8) | raw_data[3];
                vol_lev = raw_data[5];
            }
            else if (op == OP_BASICSET)
            {
                index = raw_data[0];
                state = raw_data[1];
                vo_set = (raw_data[3] << 8) | raw_data[2];
                io_set = (raw_data[5] << 8) | raw_data[4];
                ovp_set = (raw_data[7] << 8) | raw_data[6];
                ocp_set = (raw_data[9] << 8) | raw_data[8];
                isEnabled = (state == 1);
                currentVoltage = vo_set;
                currentCurrent = io_set;
            }
            else
            {
                std::cerr << "Unknown operation code: " << static_cast<int>(op) << std::endl;
            }
        }
        else
        {
            std::cerr << "CRC ERROR" << std::endl;
        }
    }
}

bool DP100::readBasicInfo()
{

    int status_read = 0;
    auto msg = generateFrame(OP_BASICINFO);
    std::vector<uint8_t> response(64);
    hid_write(device, msg.data(), msg.size());
    int res = hid_read(device, response.data(), response.size());
    if (res > 0)
    {
        checkFrame(response);
        return true;
    }
    return false;
}
bool DP100::readBasicSet()
{
    // Implementation to read the basic settings from the device
    int status_read = 0;
    auto msg = generateFrame(OP_BASICSET, std::vector<uint8_t>{SET_ACT});
    std::vector<uint8_t> response(64);
    hid_write(device, msg.data(), msg.size());
    int res = hid_read(device, response.data(), response.size());
    if (res > 0)
    {
        checkFrame(response);
        if (state > 2)
        {
            return false;
        }
        if (vo_set > 30500)
        {
            return false;
        }
        if (io_set > 5050)
        {
            return false;
        }
        return true;
    }
    return false;
}
bool DP100::readDeviceInfo()
{
    int status_read = 0;
    auto msg = generateFrame(OP_DEVICEINFO);
    std::vector<uint8_t> response(64);
    hid_write(device, msg.data(), msg.size());
    int res = hid_read(device, response.data(), response.size());
    if (res > 0)
    {
        checkFrame(response);
        return true;
    }
    return false;
}

uint16_t DP100::crc16(uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void DP100::saveSettings()
{
    std::ofstream file(settingsFile);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open settings file for writing.");
    }
    file << currentVoltage << " " << currentCurrent;
    file.close();
}

void DP100::loadSettings()
{
    std::ifstream file(settingsFile);
    if (!file.is_open())
    {
        // If the file doesn't exist, create the directory and file with default values
        std::string directory = settingsFile.substr(0, settingsFile.find_last_of('/'));
        std::system(("mkdir -p " + directory).c_str()); // Create the directory if it doesn't exist

        std::ofstream newFile(settingsFile);
        if (newFile.is_open())
        {
            currentVoltage = 0.0;
            currentCurrent = 0.0;
            newFile << currentVoltage << " " << currentCurrent;
            newFile.close();
        }
        return;
    }
    file >> currentVoltage >> currentCurrent;
    file.close();
}

double DP100::getOutputVoltage() const
{
    return vout / 1000.0f;
}

double DP100::getOutputCurrent() const
{
    return iout / 1000.0f;
}

double DP100::getSetVoltage() const
{
    return vo_set / 1000.0f;
}

double DP100::getSetCurrent() const
{
    return io_set / 1000.0f;
}