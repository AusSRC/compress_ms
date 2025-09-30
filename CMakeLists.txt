cmake_minimum_required(VERSION 3.30)
project(compress_ms)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(compress_ms compress_ms.cc)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Casacore)
find_package(MPI REQUIRED)
target_link_libraries(compress_ms PUBLIC ${CASACORE_LIBRARIES})
target_include_directories(
    compress_ms PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:include> 
    ${CASACORE_INCLUDE_DIRS}
)


install(TARGETS compress_ms)