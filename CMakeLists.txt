# Specify a minimum CMake version
cmake_minimum_required(VERSION 3.16)

# Load and run CMake from ESP-IDF directory
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set the project name
project(socket)