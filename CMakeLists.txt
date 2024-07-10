cmake_minimum_required(VERSION 3.18)

project(AOA-PC-Host VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(AOA-PC-Host main.cpp)

target_link_libraries(AOA-PC-Host usb-1.0)