cmake_minimum_required(VERSION 3.8)

project(libtopoheight VERSION 0.1)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(SYSTEM
  third_party
  third_party/rtree.c
  third_party/CDT/include
)
add_library(topoheight SHARED
  source/libtopoheight.cpp
  source/picture.cpp
  third_party/rtree.c/rtree.c
)
target_link_libraries(topoheight
  png
  CDT)
set_target_properties(topoheight PROPERTIES PREFIX "" OUTPUT_NAME topoheight)

add_subdirectory(tests)
add_subdirectory(third_party/CDT)
