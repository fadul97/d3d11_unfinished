# PATH: src/renderer/d3d11/3d/3d/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojD3D11Renderer2D
    STATIC 
    renderer2d_d3d11.cpp
)

target_link_directories(
    jojD3D11Renderer2D
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(jojD3D11Renderer2D PUBLIC jojRenderer jojD3D11Renderer dxgi.lib d3d11.lib D3DCompiler.lib)