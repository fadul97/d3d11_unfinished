# PATH: src/systems/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojSystems
    STATIC
    systems.cpp
)

add_subdirectory(camera)
add_subdirectory(light)
add_subdirectory(objects2d)
add_subdirectory(ecs)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/light/
    ${CMAKE_CURRENT_SOURCE_DIR}/objects2d/
    ${CMAKE_CURRENT_SOURCE_DIR}/ecs/
)

target_link_libraries(jojSystems PUBLIC jojCamera jojLight jojObjects2D jojECS jojMath)