# PATH: src/systems/ecs/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojECS
    STATIC
    component_map.cpp
    movement_system.cpp
    entity_manager.cpp
)

# Specify include directories for jojECS
target_include_directories(
    jojECS
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include/joj/
)

# target_link_libraries(jojECS PUBLIC jojMath)