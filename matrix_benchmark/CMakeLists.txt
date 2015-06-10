#
# <root>/matrix_benchmark/
#

cmake_minimum_required(VERSION 2.8)



project(matrix_benchmark)

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


set(matrix_benchmark_HEADERS
    "${simple_blocks_headers}"
) 

set(matrix_benchmark_SOURCES
        src/main.cpp   
        "${simple_blocks_sources}"
)

include_directories("${simple_blocks_SOURCE_DIR}/src")
include_directories("${CRUFT_DIR}/include")
include_directories("${THIRDPARTY_DIR}/tclap/include")

add_executable(matrix_benchmark "${matrix_benchmark_HEADERS}" "${matrix_benchmark_SOURCES}")


target_link_libraries(
    matrix_benchmark
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${NVTOOLSEXT_LIB}
    crufterly
)

install(TARGETS matrix_benchmark RUNTIME DESTINATION ${BIN_DIR})
