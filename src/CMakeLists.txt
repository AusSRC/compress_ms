add_executable(compress_ms compress_ms.cc)
add_executable(compareMS compareMS.cc)
target_link_libraries(compress_ms PUBLIC ${CASACORE_LIBRARIES} Boost::program_options)
target_link_libraries(compareMS PUBLIC ${CASACORE_LIBRARIES})
target_include_directories(
    compress_ms PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:include> 
    ${CASACORE_INCLUDE_DIRS}
)

target_include_directories(
    compareMS PUBLIC 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> 
    $<INSTALL_INTERFACE:include> 
    ${CASACORE_INCLUDE_DIRS}
)

install(TARGETS compress_ms)