#
# <root>/simple_blocks/
#

cmake_minimum_required(VERSION 2.8)

project(simple_blocks)

include_directories("${SHARED_DIR}/include")

add_executable(simple_blocks src/main.cpp)

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
