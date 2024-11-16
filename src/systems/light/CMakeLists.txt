# PATH: src/systems/light/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojLight
    STATIC
    light_debug.cpp
)

# Specify include directories for jojLight
target_include_directories(
    jojLight
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/joj/
)

# target_link_libraries(jojLight PUBLIC jojMath)