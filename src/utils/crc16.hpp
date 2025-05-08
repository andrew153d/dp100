#ifndef CRC16_HPP
#define CRC16_HPP

#include <cstdint>
#include <cstddef>

class CRC16 {
public:
    static uint16_t calculate(const uint8_t* data, size_t length);
};

#endif // CRC16_HPP