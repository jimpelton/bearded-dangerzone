#
# <root>/matrix_benchmark/
#

cmake_minimum_required(VERSION 2.8)




### Get sources for simple_blocks (matrix_benchmark needs block.h,cpp)
file(GLOB simple_blocks_sources "${simple_blocks_SOURCE_DIR}/src"
    "${simple_blocks_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM simple_blocks_sources
    "${simple_blocks_SOURCE_DIR}/src/main.cpp"
)

file(GLOB simple_blocks_headers "${simple_blocks_SOURCE_DIR}/src"
    "${simple_blocks_SOURCE_DIR}/src/*.h"
)


####################################################################################

project(empty_space)

set(empty_space_HEADERS
    ${simple_blocks_headers}
) 

set(empty_space_SOURCES
    ${simple_blocks_sources}
    src/main.cpp
)

include_directories("${simple_blocks_SOURCE_DIR}/src")
include_directories("${CRUFT_DIR}/include")
include_directories("${THIRDPARTY_DIR}/tclap/include")

add_executable(empty_space "${empty_space_HEADERS}" "${empty_space_SOURCES}")

target_link_libraries( empty_space
   ${OPENGL_LIBRARIES}
   ${GLFW_LIBRARIES}
   ${GLEW_LIBRARIES}
   crufterly
)

install(TARGETS empty_space RUNTIME DESTINATION ${BIN_DIR})
