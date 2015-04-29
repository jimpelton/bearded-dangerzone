#
# <root>
#

cmake_minimum_required(VERSION 2.8.4)
project(bzone)


if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++14 -Wall -fdiagnostics-color=auto")
endif (UNIX)

# Windows requires a few extra definitions.
if (WIN32)
    # I'm not sure if this projects local module path should be  searched
    # under non-win32 or not...so this line might need to be lifted outside
    # of this if.
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
        ${CMAKE_MODULE_PATH})
        
    add_definitions(-DGLEW_STATIC)
    #add_definitions(-DGLFW_DLL)
    add_definitions(-DNOMINMAX)
    
endif (WIN32)


set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shared/")
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/")

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)


#find_package(Boost REQUIRED program_options)
#find_package(Freetype REQUIRED)

message(STATUS "SHARED_DIR: ${SHARED_DIR}")
message(STATUS "BIN_DIR: ${BIN_DIR}")

# All of this include business should probably go down into the 
# top levels of the project subdirectories some day, but 
# that day is not today!

# On Windows I guess OPENGL_INCLUDE_DIR isn't defined,
# on Unix however, the opengl include dirs aren't always
# in the same place, so we explicitly include them.
if (UNIX)
    include_directories("${OPENGL_INCLUDE_DIR}")
endif(UNIX)


if (WIN32)
    include_directories("${GLFW_INCLUDE_DIRS}")
endif(WIN32)

include_directories("${GLEW_INCLUDE_DIR}")
include_directories("${GLM_INCLUDE_DIR}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shared")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simple_blocks")


# Projects temporaily not (or ever) being used.
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/template")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/instancing")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/simplevr")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/scene_graph")




