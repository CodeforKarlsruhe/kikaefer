# for https://github.com/Tencent/ncnn/wiki/how-to-build#build-for-arm-cortex-a-family-with-cross-compiling
# Specify the CMake version (optional but recommended)
cmake_minimum_required(VERSION 3.0)

# Set the cross-compilation environment
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# Set the cross-compilation toolchain prefix
SET(CMAKE_C_COMPILER   /home/kugel/temp/m5/unitv2/buildroot-2020.08.2/output/host/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER /home/kugel/temp/m5/unitv2/buildroot-2020.08.2/output/host/bin/arm-linux-g++)

# Set other toolchain-related variables if needed (e.g., flags, libraries, etc.)
SET(CMAKE_FIND_ROOT_PATH /home/kugel/temp/m5/unitv2/buildroot-2020.08.2/output/host/arm-buildroot-linux-gnueabihf/sysroot/)

# Set any other platform-specific configurations or library paths if needed
# SET(CMAKE_LIBRARY_PATH /path/to/your/arm-linux-gnueabihf/lib)
# SET(CMAKE_INCLUDE_PATH /path/to/your/arm-linux-gnueabihf/include)

