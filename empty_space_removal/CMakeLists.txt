#
# <root>/empty_space_removal
#

cmake_minimum_required(VERSION 2.8)

### Get sources for simple_blocks (matrix_benchmark needs block.h,cpp)
# file(GLOB simple_blocks_sources "${simple_blocks_SOURCE_DIR}/src"
#     "${simple_blocks_SOURCE_DIR}/src/*.cpp"
# )

# list(REMOVE_ITEM simple_blocks_sources
#     "${simple_blocks_SOURCE_DIR}/src/main.cpp"
# )

# file(GLOB simple_blocks_headers "${simple_blocks_SOURCE_DIR}/src"
#     "${simple_blocks_SOURCE_DIR}/src/*.h"
# )


####################################################################################

project(empty_space_removal)

set(empty_space_removal_HEADERS
  src/binaryvolume.h
  src/cmdline.h
  # ${simple_blocks_headers}
) 

set(empty_space_removal_SOURCES
    src/main.cpp
    src/cmdline.cpp
    # ${simple_blocks_sources}
)

#include_directories("${simple_blocks_SOURCE_DIR}/src")
include_directories("${CRUFT_DIR}/include")
include_directories("${THIRDPARTY_DIR}/tclap/include")

add_executable(empty_space_removal "${empty_space_removal_HEADERS}" "${empty_space_removal_SOURCES}")

target_link_libraries(empty_space_removal
   ${OPENGL_LIBRARIES}
   ${GLFW_LIBRARIES}
   ${GLEW_LIBRARIES}
   crufterly
)

install(TARGETS empty_space_removal RUNTIME DESTINATION ${BIN_DIR})
