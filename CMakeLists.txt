cmake_minimum_required(VERSION 3.10)

project(dp100-control)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

file(GLOB SOURCES "src/*.cpp" "src/utils/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(HIDAPI REQUIRED hidapi-libusb)

add_executable(dp100-control ${SOURCES})

# Link the hidapi library
target_link_libraries(dp100-control PRIVATE ${HIDAPI_LIBRARIES})
include_directories(${HIDAPI_INCLUDE_DIRS})