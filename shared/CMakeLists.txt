#
# shared/CMakeLists.txt
#

cmake_minimum_required(VERSION 2.8)

project(shared LANGUAGES CXX)

# Enables gl_check() macros.
add_definitions(-DBD_DEBUG)


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(src/bd)

add_library(shared STATIC "${bd_SOURCES}" "${bd_HEADERS}" )


