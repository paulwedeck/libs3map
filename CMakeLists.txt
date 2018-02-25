cmake_minimum_required(VERSION 3.2.2)
project(libs3map VERSION 1.0.0)

include(CTest)
include(TestBigEndian)
include(CMakePackageConfigHelpers)

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/bin")
set(CPACK_PACKAGE_NAME "libs3map")
set(CPACK_PACKAGE_VERSION ${libs3map_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${libs3map_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${libs3map_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${libs3map_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libs3util (=1.0.0)")
include(CPack)

TEST_BIG_ENDIAN(IS_BE)

find_package(s3util REQUIRED)

configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)
configure_file(libs3map.pc.in ${PROJECT_BINARY_DIR}/libs3map.pc @ONLY)

include_directories(src ${PROJECT_BINARY_DIR} ${s3util_INCLUDE_DIRS})


#s3map
file(GLOB_RECURSE s3map_SOURCES "src/*.[ch]")

add_library(s3map SHARED ${s3map_SOURCES})
target_link_libraries(s3map s3util)
target_include_directories(s3map PUBLIC $<INSTALL_INTERFACE:include/s3map>)

add_executable(cli cli.c)
target_link_libraries(cli s3map)

write_basic_package_version_file(${PROJECT_BINARY_DIR}/s3mapConfigVersion.cmake COMPATIBILITY ExactVersion)
configure_package_config_file(s3mapConfig.cmake.in s3mapConfig.cmake INSTALL_DESTINATION lib/cmake/s3map PATH_VARS CMAKE_INSTALL_PREFIX CMAKE_SOURCE_DIR)

install(TARGETS s3map EXPORT s3mapTarget DESTINATION lib)
install(EXPORT s3mapTarget DESTINATION lib/cmake/s3map)

#for find_package on build dirs
export(TARGETS s3map FILE s3mapTarget.cmake)

install(FILES src/types.h src/s3map.h DESTINATION include/s3map)
install(FILES ${PROJECT_BINARY_DIR}/libs3map.pc DESTINATION lib/pkgconfig)
install(FILES ${PROJECT_BINARY_DIR}/s3mapConfigVersion.cmake ${PROJECT_BINARY_DIR}/s3mapConfig.cmake DESTINATION lib/cmake/s3map)
install(FILES LICENSE DESTINATION share/doc/libs3map)

find_package(glfw3)
find_package(OpenGL)


if(glfw3_FOUND AND OpenGL_FOUND)
	add_executable(minimap minimap.c)
	target_link_libraries(minimap s3map ${OPENGL_LIBRARIES} glfw m)
	target_include_directories(minimap PUBLIC ${OPENGL_INCLUDE_DIRS} ${glfw3_INCLUDE_DIRS})
endif()

