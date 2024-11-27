# PATH: include/joj/renderer/d3d11/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../)

if(WIN32)
	add_subdirectory(3d)
endif()