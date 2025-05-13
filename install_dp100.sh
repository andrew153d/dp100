#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Define variables
RULES_FILE="99-dp100.rules"
REPO_URL="https://github.com/andrew153d/dp100.git"  # Replace with the actual repository URL
BUILD_DIR="build"
INSTALL_DIR="/usr/local/bin"

# Step 0: Install required dependencies
if ! pkg-config --exists hidapi-libusb; then
    echo "Installing hidapi-libusb..."
    sudo apt-get update
    sudo apt-get install -y libhidapi-libusb0 libhidapi-dev
else
    echo "hidapi-libusb is already installed."
fi

# Step 2: Clone the repository
TEMP_DIR=$(mktemp -d)
echo "Cloning the repository into $TEMP_DIR..."
git clone $REPO_URL $TEMP_DIR/dp100

# Step 1: Install the rules file
if [ -f "$TEMP_DIR/dp100/$RULES_FILE" ]; then
    echo "Installing udev rules..."
    sudo cp $TEMP_DIR/dp100/$RULES_FILE /etc/udev/rules.d/
    sudo udevadm control --reload-rules
    sudo udevadm trigger
else
    echo "Rules file not found: $TEMP_DIR/dp100/$RULES_FILE"
    exit 1
fi

# Step 3: create the settings directory
SETTINGS_DIR="$HOME/.dp100/"
if [ ! -d "$SETTINGS_DIR" ]; then
    echo "Creating settings directory..."
    mkdir -p $SETTINGS_DIR
fi

touch "$SETTINGS_DIR/dp100_settings.txt"

# Step 4: Build the project
cd $TEMP_DIR/dp100
if [ -d "$BUILD_DIR" ]; then
    rm -rf $BUILD_DIR
fi
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake ..
make

# Step 5: Install the project
sudo make install

# Step 6: Clean up
cd ~
echo "Removing temporary files..."
rm -rf $TEMP_DIR

# Completion message
echo "DP100 project installed successfully."
