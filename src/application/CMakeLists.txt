# PATH: src/application/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(jojApplication STATIC app.cpp)

# Specify include directories for jojApplication
target_include_directories(
    jojApplication
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/
)