#
# <root>/simple_blocks/
#

cmake_minimum_required(VERSION 2.8)

project(simple_blocks)

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

include_directories("${SHARED_DIR}/include")  
#include_directories("${GLFW_INCLUDE_DIRS}")
#include_directories("${Boost_INCLUDE_DIR}")
include_directories("${THIRDPARTY_DIR}/tclap/include")

set(simple_blocks_HEADERS src/block.h src/cmdline.h src/texture.h)
set(simple_blocks_SOURCES src/main.cpp src/block.cpp src/cmdline.cpp src/texture.cpp)

add_executable(simple_blocks "${simple_blocks_HEADERS}" "${simple_blocks_SOURCES}")


target_link_libraries(
    simple_blocks
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
#    ${FREETYPE_LIBRARIES}
    shared
)



add_custom_command(TARGET simple_blocks POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:simple_blocks>/shaders)

install(TARGETS simple_blocks RUNTIME DESTINATION ${BIN_DIR})
