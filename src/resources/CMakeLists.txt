# PATH: src/resources/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojResources
    STATIC
    resources.cpp
 )

add_subdirectory(geometry)

# Specify include directories for jojResources
target_include_directories(
    jojResources
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/
)

target_link_directories(
    joj
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/math/
    ${CMAKE_CURRENT_SOURCE_DIR}/geometry/
)

target_link_libraries(jojResources PUBLIC jojGeometry jojMath)