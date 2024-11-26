# PATH: app/CMakeList.txt
cmake_minimum_required(VERSION 3.22)

add_executable(
	jojApp
	main.cpp
	
	# Demo App
	demos/istancing_and_culling_demo.cpp
  )
# => demos folder:
# box_demo.cpp shapes_demo.cpp hills_demo.cpp skull_demo.cpp
# waves.cpp waves_demo.cpp lit_waves_demo.cpp lit_skull_demo.cpp
# crate_demo.cpp textured_hills_waves_demo.cpp blend_demo.cpp
# mirror_demo.cpp

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET jojApp PROPERTY CXX_STANDARD 20)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)

target_link_directories(jojApp PUBLIC src)

target_link_directories(
	jojApp
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../src/
)

target_link_libraries(jojApp PUBLIC joj)