# PATH: src/platform/win32/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_library(jojWin32Platform STATIC window_win32.cpp input_win32.cpp timer_win32.cpp)

target_link_directories(
    jojWin32Platform 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(jojWin32Platform PUBLIC jojPlatform User32.lib Gdi32.lib winmm.lib)