#
# <root>
#

cmake_minimum_required(VERSION 2.8)
project(bzone)


if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++14 -Wall -fdiagnostics-color=auto")
endif (UNIX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH}
)

# Windows requires a few extra definitions.
if (WIN32)
    add_definitions(-DGLEW_STATIC)
    add_definitions(-DNOMINMAX)
endif (WIN32)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CRUFT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cruft/")
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/")


if (UNIX)
    include_directories("${OPENGL_INCLUDE_DIR}")
endif(UNIX)

include_directories("${GLEW_INCLUDE_DIR}")
include_directories("${GLM_INCLUDE_DIR}")
include_directories("${GLFW_INCLUDE_DIRS}")

add_subdirectory("${CRUFT_DIR}")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simple_blocks")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/matrix_benchmark")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/empty_space_removal")

# Projects temporaily not (or ever) being used.
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/template")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/instancing")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simplevr")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/scene_graph")


