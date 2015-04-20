#
# <root>/simple_blocks/
#

cmake_minimum_required(VERSION 2.8)

project(simple_blocks)

include_directories("${SHARED_DIR}/include")


set(simple_blocks_HEADERS src/block.h src/cmdline.h)
set(simple_blocks_SOURCES src/main.cpp src/block.cpp src/cmdline.cpp)

add_executable(simple_blocks "${simple_blocks_HEADERS}" "${simple_blocks_SOURCES}")

target_link_libraries(
    simple_blocks
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    ${Boost_LIBRARIES}
#   ${FREETYPE_LIBRARIES}
    shared
    )


install(TARGETS simple_blocks RUNTIME DESTINATION ${BIN_DIR})
