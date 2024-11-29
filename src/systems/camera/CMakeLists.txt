# PATH: src/systems/camera/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojCamera
    STATIC
    free_camera_old.cpp
    free_camera.cpp
)

# Specify include directories for jojCamera
target_include_directories(
    jojCamera
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/joj/
)

# target_link_libraries(jojCamera PUBLIC jojMath)