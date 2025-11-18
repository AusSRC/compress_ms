cmake_minimum_required(VERSION 3.20)
project(compress_ms)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Casacore)
find_package(MPI REQUIRED)
find_package(Python REQUIRED)
find_package(Boost 1.83 COMPONENTS program_options REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)