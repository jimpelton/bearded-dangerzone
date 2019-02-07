
#
# <root>/test/CMakeLists.txt
#

cmake_minimum_required(VERSION 2.8)
include_directories("../include" "./catch")
message(STATUS "Test resource directory: ${CRUFT_RESOURCE_DIR}")
add_definitions(-DRESOURCE_FOLDER="${CRUFT_RESOURCE_DIR}")
add_subdirectory("test_io")
add_subdirectory("test_parsedat")
add_subdirectory("test_util")
add_subdirectory("test_volume")
#add_subdirectory("test_tbb")
add_subdirectory("test_datastructure")

