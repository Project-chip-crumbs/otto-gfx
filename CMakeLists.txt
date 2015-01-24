cmake_minimum_required(VERSION 2.8)
project(otto_gfx)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -stdlib=libc++")

include_directories(
  "deps/Choreograph/src"
  "deps/nanosvg/src"
  "src")

include_directories(SYSTEM
  "/opt/vc/include") # VideoCore

file(GLOB deps_src "deps/Choreograph/src/choreograph/*.cpp")
file(GLOB src "src/*.cpp")

add_library(otto_gfx ${deps_src} ${src})
