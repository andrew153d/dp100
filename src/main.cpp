#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include "dp100.hpp"

void printUsage()
{
    std::cout << "Usage:\n"
              << "  dp100 --v <voltage> --a <current>  Set voltage and current\n"
              << "  dp100 --enable                      Enable the power supply\n"
              << "  dp100 --disable                     Disable the power supply\n"
              << "  dp100 --status                      Retrieve the status\n";

    exit(0);
}

int main(int argc, char *argv[])
{
    DP100 dp100;

    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    // Parse the command line arguments
    // each element will be a vector where the first element is the command and the rest are the arguments
    std::vector<std::vector<std::string>> args = {};

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg.find("-") != std::string::npos)
        {
            args.push_back({arg});
        }
        else
        {
            if(args.empty() || args.back().empty())
            {
                std::cerr << "Error: Argument without command.\n";
                printUsage();
                return 1;
            }
            args.back().push_back(arg);
        }
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

    for (const auto &arg : args)
    {
        if (arg[0] == "--v" && arg.size() == 2)
        {
            double voltage = std::stod(arg[1]);
            dp100.setVoltage(voltage);
        }
        else if (arg[0] == "--a" && arg.size() == 2)
        {
            double current = std::stod(arg[1]);
            dp100.setCurrent(current);
        }
        else if ((arg[0] == "--enable" || arg[0] == "--e") && arg.size() == 3)
        {
            double voltage_set = std::stod(arg[1]);
            //printf("Setting voltage to %f V\n", voltage_set);
            double current_set = std::stod(arg[2]);
            //printf("Setting current to %f A\n", current_set);
            
            dp100.setVoltage(voltage_set);
            dp100.setCurrent(current_set);
            dp100.enable();
        }
        else if (arg[0] == "--disable" || arg[0] == "--d")
        {
            dp100.disable();
        }
        else if (arg[0] == "--status" || arg[0] == "--s")
        {
            while(1)
            {
                dp100.readBasicInfo();
                printf("\r");
                fflush(stdout);
                printf("Voltage: %f V, Current: %f A", dp100.getOutputVoltage(), dp100.getOutputCurrent());
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        else
        {
            printUsage();
            return 1;
        }
    }

    return 0;
}