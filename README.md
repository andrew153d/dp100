# dp100 Power Supply Control

This project provides a command-line interface for controlling the DP100 power supply. It allows users to set voltage and current, enable or disable the power supply, and retrieve status information.

## Features

- Connect and disconnect from the DP100 device.
- Set voltage and current values.
- Enable or disable the device.
- Retrieve detailed device status and information.

## Installation

To install the DP100 Control Project, follow these steps:

1. Download the installation script using `wget`:

   ```bash
   wget https://raw.githubusercontent.com/andrew153d/dp100/main/install_dp100.sh
   ```

2. Make the script executable:

   ```bash
   chmod +x install_dp100.sh
   ```

3. Run the installation script:

   ```bash
   ./install_dp100.sh
   ```

Alternatively, you can clone the repository and build the project manually:

1. Clone the repository:

   ```bash
   git clone <repository-url>
   cd dp100
   ```

2. Create a build directory and navigate into it:

   ```bash
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:

   ```bash
   cmake ..
   ```

4. Build the project:

   ```bash
   make
   ```

## Usage

After building the project, you can use the `dp100` command followed by the desired options:

- To set the voltage and current:

  ```bash
  ./dp100 --v <voltage> --a <current>
  ```

  Example:

  ```bash
  ./dp100 --v 12.0 --a 1.1
  ```

- To enable the power supply:

  ```bash
  ./dp100 --enable
  ```

- To disable the power supply:

  ```bash
  ./dp100 --disable
  ```

- To retrieve the current status:

  ```bash
  ./dp100 --status
  ```

## License

This project is licensed under the MIT License. See the LICENSE file for more details.