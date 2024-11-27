# PATH: src/renderer/d3d11/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojD3D11Renderer
    STATIC
    context_d3d11.cpp
    vertex_buffer_d3d11.cpp
    index_buffer_d3d11.cpp
    constant_buffer_d3d11.cpp
    shader_d3d11.cpp
    DDSTextureLoader11.cpp
)

# Specify include directories for jojRenderer
target_include_directories(
    jojD3D11Renderer
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/joj/
)

if(WIN32)
    add_subdirectory(2d)
    add_subdirectory(3d)

    target_link_directories(
        jojD3D11Renderer 
        PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/../platform/
        ${CMAKE_CURRENT_SOURCE_DIR}/2d/
        ${CMAKE_CURRENT_SOURCE_DIR}/3d/
    )

    target_link_libraries(jojD3D11Renderer PUBLIC jojPlatform jojD3D11Renderer2D jojD3D11Renderer3D)
endif()