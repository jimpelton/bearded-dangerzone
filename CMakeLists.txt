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


#### Intel TBB Library ##################################################
#if(NOT DEFINED TBB_INCLUDE_DIR)
#    set(TBB_INCLUDE_DIR)
#endif()
#set(TBB_INCLUDE_DIR ${${TBB_INCLUDE_DIR}} CACHE PATH "Cruft includes directory path")
#
#if(NOT DEFINED TBB_LIBRARY_DIR)
#    set(TBB_LIBRARY_DIR)
#endif()
#set(TBB_LIBRARY_DIR ${${TBB_LIBRARY_DIR}} CACHE PATH "Directory with intel tbb libs.")


#############################################################################
if (UNIX)
    set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} \
            -std=c++17 \
            -pthread \
            -Wall \
            -fdiagnostics-color=auto")
endif (UNIX)

# Windows requires a few extra definitions.
if (WIN32)
    add_definitions(-DGLEW_STATIC)
    add_definitions(-DNOMINMAX)
endif (WIN32)


set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libcruft")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simple_blocks")

#if (UNIX)
 #   include_directories("${OPENGL_INCLUDE_DIR}")
#endif(UNIX)

#include_directories("${GLEW_INCLUDE_DIR}")
#include_directories("${GLM_INCLUDE_DIR}")
#include_directories("${GLFW_INCLUDE_DIRS}")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/matrix_benchmark")

# Projects temporaily not (or ever) being used.
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/template")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/instancing")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simplevr")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/scene_graph")

if(CREATE_TESTS)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif(CREATE_TESTS)

