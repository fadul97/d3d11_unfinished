# PATH: src/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

if(WIN32)
    add_library(
        joj
        STATIC
        error_code.cpp
        logger_win32.cpp
   )
endif()

# Specify include directories for joj
target_include_directories(
    joj
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/joj/
)