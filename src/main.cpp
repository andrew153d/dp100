#include <iostream>
#include <string>
#include <cstdlib>
#include "dp100.hpp"

void printUsage()
{
    std::cout << "Usage:\n"
              << "  dp100 --v <voltage> --a <current>  Set voltage and current\n"
              << "  dp100 --enable                      Enable the power supply\n"
              << "  dp100 --disable                     Disable the power supply\n"
              << "  dp100 --status                      Retrieve the status\n";
}

int main(int argc, char *argv[])
{
    DP100 dp100;

    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    // Initialize HIDAPI
    if (hid_init() != 0)
    {
        std::cerr << "Failed to initialize HIDAPI.\n";
        return 1;
    }

    if (!dp100.connect())
    {
        std::cerr << "Failed to connect to DP100 power supply.\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "--v" && argc == 4)
    {
        double voltage = std::stod(argv[2]);
        double current = std::stod(argv[3]);
        dp100.setVoltage(voltage);
        dp100.setCurrent(current);
        std::cout << "Voltage set to " << voltage << " V and current set to " << current << " A.\n";
    }
    else if (command == "--enable")
    {
        dp100.enable();
        std::cout << "Power supply enabled.\n";
    }
    else if (command == "--disable")
    {
        dp100.disable();
        std::cout << "Power supply disabled.\n";
    }
    else if (command == "--status")
    {
        dp100.getStatus();
    }
    else
    {
        printUsage();
        return 1;
    }

    return 0;
}