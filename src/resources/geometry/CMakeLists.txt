# src/resources/geometry/CMakeLists.txt
cmake_minimum_required(VERSION 3.8)
project(joj)

add_library(
    jojGeometry
    STATIC
    geometry_vertex.cpp
    geometry.cpp
    cube.cpp
    sphere.cpp
    cylinder.cpp
)

# Specify include directories for jojGeometry
target_include_directories(
    jojGeometry
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/joj/
)

# target_link_libraries(jojGeometry PUBLIC jojMath)