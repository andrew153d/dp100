#include "crc16.hpp"

uint16_t crc16(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF; // Initial value
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (size_t j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001; // Polynomial
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}