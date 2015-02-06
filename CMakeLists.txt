cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
      ${CMAKE_MODULE_PATH})

if (UNIX)
#   set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
#   set(CMAKE_AR "/usr/bin/llvm-ar")
#   set(CMAKE_LINKER "/usr/bin/llvm-ld")
#   set(CMAKE_NM "/usr/bin/llvm-nm")
#   set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
#   set(CMAKE_RANLIB "/usr/bin/llvm-ranlib")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11 -Wall -fdiagnostics-color=always")

endif (UNIX)

if (WIN32)
    add_definitions(-DGLEW_STATIC)
    add_definitions(-DGLFW_DLL)
    add_definitions(-DNOMINMAX)
endif (WIN32)

set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shared")
#message(STATUS "SHARED_INCLUDES_DIR: ${SHARED_DIR}")
#message(STATUS "BIN_DIR: ${BIN_DIR}")

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
#find_package(Boost REQUIRED program_options)
#find_package(Freetype REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})

include_directories("${SHARED_DIR}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/instancing")
