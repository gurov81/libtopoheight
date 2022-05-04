cmake_minimum_required(VERSION 3.8)

project(libtopoheight VERSION 0.1)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(SYSTEM
  third_party
  third_party/delaunator-cpp/include
)
add_library(topoheight SHARED source/libtopoheight.cpp)
set_target_properties(topoheight PROPERTIES PREFIX "" OUTPUT_NAME topoheight)

add_subdirectory(tests)
