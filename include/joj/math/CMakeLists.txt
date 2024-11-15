# PATH: include/joj/math/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(jojMath INTERFACE .)

# TODO: if interface, remove 'legacy' include_directories()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

if(WIN32)
    target_link_libraries(jojMath INTERFACE dxgi.lib d3d11.lib)
endif()