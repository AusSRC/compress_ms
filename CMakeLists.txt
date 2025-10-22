cmake_minimum_required(VERSION 3.30)
project(compress_ms)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Casacore)
find_package(MPI REQUIRED)

add_subdirectory(src)