#
# <root>
#

cmake_minimum_required(VERSION 2.8)
project(subvol)

if(NOT DEFINED CREATE_TESTS)
	set(CREATE_TESTS FALSE)
endif()	
set(CREATE_TESTS ${${CREATE_TESTS}} CACHE BOOL "True if cmake should create unit test projects")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})


################################################################################
# Libcrufterly
if(NOT DEFINED CRUFT_INCLUDE_DIR)
    set(CRUFT_INCLUDE_DIR)
endif()
set(CRUFT_INCLUDE_DIR ${${CRUFT_INCLUDE_DIR}} CACHE PATH "Cruft includes directory path")

if(NOT DEFINED CRUFT_DEBUG_LIBRARIES)
    set(CRUFT_DEBUG_LIBRARIES)
endif()
set(CRUFT_DEBUG_LIBRARIES ${${CRUFT_DEBUG_LIBRARIES}} CACHE FILEPATH "Cruft library file (crufterly.a).")

if(NOT DEFINED CRUFT_RELEASE_LIBRARIES)
    set(CRUFT_RELEASE_LIBRARIES)
endif()
set(CRUFT_RELEASE_LIBRARIES ${${CRUFT_RELEASE_LIBRARIES}} CACHE FILEPATH "Cruft library file (crufterly.a).")



if (UNIX)
    set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} \
            -std=c++14 \
            -pthread \
            -Wall \
            -fdiagnostics-color=auto")
endif (UNIX)

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
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/")


if (UNIX)
    include_directories("${OPENGL_INCLUDE_DIR}")
endif(UNIX)

include_directories("${GLEW_INCLUDE_DIR}")
include_directories("${GLM_INCLUDE_DIR}")
include_directories("${GLFW_INCLUDE_DIRS}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simple_blocks")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/rawhist")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/matrix_benchmark")

# Projects temporaily not (or ever) being used.
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/template")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/instancing")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simplevr")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/scene_graph")

if(CREATE_TESTS)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif(CREATE_TESTS)

