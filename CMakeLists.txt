cmake_minimum_required(VERSION 3.18)

project(AOA-PC-Host VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(Include)

file(GLOB_RECURSE SRC_FILES Source/*.cpp)

add_executable(AOA-PC-Host main.cpp ${SRC_FILES})

target_link_libraries(AOA-PC-Host usb-1.0)