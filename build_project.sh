#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Navigate to the project directory
cd "$(dirname "$0")"

# Create the build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Navigate to the build directory
cd build

# Run cmake to configure the project
cmake ..

# Build the project using make
make

# Notify the user of successful build
echo "Build completed successfully."
