#ifndef DP100_HPP
#define DP100_HPP

#include <string>

class DP100 {
public:
    DP100();
    ~DP100();

    void setVoltage(float voltage);
    void setCurrent(float current);
    void enable();
    void disable();
    std::string getStatus();

private:
    void sendCommand(const std::string& command);
    std::string readResponse();
};

#endif // DP100_HPP