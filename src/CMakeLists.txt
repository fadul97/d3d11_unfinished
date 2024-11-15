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

#Add subdirectories
add_subdirectory(platform)
add_subdirectory(renderer)
add_subdirectory(resources)
add_subdirectory(application)

# Specify library directories for joj
target_link_directories(
    joj
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer/
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/
    ${CMAKE_CURRENT_SOURCE_DIR}/application/
)

target_link_libraries(
    joj
    PUBLIC
    jojPlatform
    jojRenderer
    jojApplication
    jojMath
    jojResources
)