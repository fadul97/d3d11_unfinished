cmake_minimum_required(VERSION 3.22)
project(joj)

add_executable(jojApp main.cpp)

include_directories(include)
add_subdirectory(src)

target_link_directories(jojApp PUBLIC src)

target_link_libraries(jojApp PUBLIC joj)