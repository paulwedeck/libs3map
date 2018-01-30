cmake_minimum_required(VERSION 3.2.2)
project(libs3map)

add_subdirectory(libs3util)

include(CPack)
include(CTest)
include(TestBigEndian)

set(CMAKE_C_FLAGS "-Wall -Wno-pointer-sign")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/bin")
set(CPACK_PACKAGE_NAME "libs3map")

TEST_BIG_ENDIAN(IS_BE)

configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)

include_directories(src ${PROJECT_BINARY_DIR} libs3util/src)


#s3map
file(GLOB_RECURSE s3map_SOURCES "src/*.[ch]")

add_library(s3map SHARED ${s3map_SOURCES})
target_link_libraries(s3map s3util)

