cmake_minimum_required(VERSION 2.8.4)
project(inst)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
      ${CMAKE_MODULE_PATH})

if (UNIX)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11 -Wall")

    set(CMAKE_AR "/usr/bin/llvm-ar")
    set(CMAKE_LINKER "/usr/bin/llvm-ld")
    set(CMAKE_NM "/usr/bin/llvm-nm")
    set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
    set(CMAKE_RANLIB "/usr/bin/llvm-ranlib")
endif (UNIX)

if (WIN32)
    add_definitions(-DGLEW_STATIC)
    add_definitions(-DGLFW_DLL)
    add_definitions(-DNOMINMAX)
endif (WIN32)

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
#include_directories(${Boost_INCLUDE_DIRS})
#include_directories(${FREETYPE_INCLUDE_DIRS})

#message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
#message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
#message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
#message(STATUS "Freetype_INCLUDE_DIRS: ${FREETYPE_INCLUDE_DIRS}")
#message(STATUS "Freetype_LIBRARIES: ${FREETYPE_LIBRARIES}")


set(HEADER_FILES src/BBox.h 
                 src/mesh.h
				 src/tiny_obj_loader/tiny_obj_loader.h)

set(SOURCE_FILES src/instmain.cpp 
				 src/BBox.cpp 
				 src/mesh.cpp
				 src/tiny_obj_loader/tiny_obj_loader.cc)

add_executable(inst ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(inst
                        ${OPENGL_LIBRARIES}
                        ${GLFW_LIBRARIES}
                        ${GLEW_LIBRARIES}
#                        ${Boost_PROGRAM_OPTIONS_LIBRARIES}
#                        ${Boost_LIBRARIES} 
#                        ${FREETYPE_LIBRARIES}
                        )

# install(TARGETS textureSlicesTest RUNTIME DESTINATION ${BIN_DIR})
