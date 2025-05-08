# dp100 Power Supply Control

This project provides a command-line interface for controlling the DP100 power supply. It allows users to set voltage and current, enable or disable the power supply, and retrieve status information.

## Features

- Set voltage and current using command-line arguments.
- Enable or disable the power supply.
- Retrieve the current status of the power supply.

## Installation

1. Clone the repository:
   ```
   git clone <repository-url>
   cd dp100-control
   ```

2. Create a build directory and navigate into it:
   ```
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   make
   ```

## Usage

After building the project, you can use the `dp100` command followed by the desired options:

- To set the voltage and current:
  ```
  ./dp100 --v <voltage> --a <current>
  ```
  Example:
  ```
  ./dp100 --v 12.0 --a 1.1
  ```

- To enable the power supply:
  ```
  ./dp100 --enable
  ```

- To disable the power supply:
  ```
  ./dp100 --disable
  ```

- To retrieve the current status:
  ```
  ./dp100 --status
  ```

## License

This project is licensed under the MIT License. See the LICENSE file for more details.