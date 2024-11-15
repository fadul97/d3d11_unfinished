# PATH: src/systems/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojSystems
    STATIC
    systems.cpp
 )

add_subdirectory(camera)

# Specify include directories for jojSystems
target_include_directories(
    jojSystems
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/
)

target_link_directories(
    joj
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/math/
    ${CMAKE_CURRENT_SOURCE_DIR}/camera/
)

target_link_libraries(jojSystems PUBLIC jojCamera jojMath)