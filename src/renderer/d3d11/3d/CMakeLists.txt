# PATH: src/renderer/d3d11/3d/3d/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(
    jojD3D11Renderer3D
    STATIC 
    renderer_d3d11.cpp
    renderable_object_d3d11.cpp
    scene_d3d11.cpp
    render_state_d3d11.cpp
    blur_filter_d3d11.cpp
    texture_manager_d3d11.cpp
    mesh_d3d11.cpp
    resource_loader_d3d11.cpp
    basic_model_d3d11.cpp
)

target_link_directories(
    jojD3D11Renderer3D
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(jojD3D11Renderer3D PUBLIC jojRenderer jojD3D11Renderer dxgi.lib d3d11.lib D3DCompiler.lib)