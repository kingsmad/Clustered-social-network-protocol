cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project (socio)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-sign-compare -std=c++11")

add_subdirectory(src)

add_executable(run main.cc)

target_link_libraries(run graph)

file(GLOB all_source_file ./src*.h ./src/*.cpp ./src/*.cc)
add_custom_target(socio_format COMMAND clang-format -style=file -i ${all_source_file})

add_dependencies(run socio_format)
