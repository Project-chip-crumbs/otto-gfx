cmake_minimum_required(VERSION 2.8)
project(otto_gfx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
  deps/nanosvg/src
  deps/glm/glm
  lib
  src)

include_directories(SYSTEM ${VC_SDK}/include) # VideoCore

file(GLOB src "src/*.cpp")
file(GLOB includes
  "src/*.hpp"
  "deps/nanosvg/src/nanosvg.h"
)

add_library(otto_gfx SHARED ${src})
install ( FILES "${includes}" DESTINATION ${CMAKE_INSTALL_PREFIX}/include/otto-gfx )
install ( TARGETS otto_gfx EXPORT otto_gfx DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )