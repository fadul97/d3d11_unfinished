# PATH: include/joj/CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(joj)

add_subdirectory(platform)
add_subdirectory(renderer)
add_subdirectory(math)
add_subdirectory(resources)
add_subdirectory(systems)
add_subdirectory(application)