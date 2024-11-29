# PATH: src/systems/objects/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojObjects2D
    STATIC
    object2d.cpp
)

# Specify include directories for jojObjects2D
target_include_directories(
    jojObjects2D
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/joj/
)

# target_link_libraries(jojObjects2D PUBLIC jojMath)