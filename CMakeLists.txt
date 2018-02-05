cmake_minimum_required(VERSION 3.2.2)
project(libs3map)

add_subdirectory(libs3util)

include(CPack)
include(CTest)
include(TestBigEndian)

set(CMAKE_C_FLAGS "-Wall")
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

add_executable(cli cli.c)
target_link_libraries(cli s3map)

install(TARGETS s3map DESTINATION lib)
install(FILES src/s3map.h DESTINATION include/)

find_package(glfw3)
find_package(OpenGL)


if(glfw3_FOUND AND OpenGL_FOUND)
	add_executable(minimap minimap.c)
	target_link_libraries(minimap s3map ${OPENGL_LIBRARIES} glfw m)
	target_include_directories(minimap PUBLIC ${OPENGL_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS})
endif()
