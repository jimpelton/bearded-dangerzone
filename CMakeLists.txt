#
# <root>
#

cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
      ${CMAKE_MODULE_PATH})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++14 -Wall -fdiagnostics-color=auto")

if (WIN32)
    add_definitions(-DGLEW_STATIC)
    add_definitions(-DGLFW_DLL)
    add_definitions(-DNOMINMAX)
endif (WIN32)


set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shared/")
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/")

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Boost REQUIRED program_options)
#find_package(Freetype REQUIRED)

message(STATUS "SHARED_DIR: ${SHARED_DIR}")
message(STATUS "BIN_DIR: ${BIN_DIR}")

include_directories("${OPENGL_INCLUDE_DIR}")
#include_directories("${GLFW_INCLUDE_DIR}")
include_directories("${GLEW_INCLUDE_DIR}")
include_directories("${GLM_INCLUDE_DIR}")
#include_directories("${SHARED_DIR}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shared")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/template")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/instancing")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simplevr")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/scene_graph")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simple_blocks")



