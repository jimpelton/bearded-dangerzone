#
# <root>/test/simple_blocks_test
#

cmake_minimum_required(VERSION 2.8)

project(simple_blocks_test)

file(GLOB simple_blocks_sources "${simple_blocks_SOURCE_DIR}/src"
    "${simple_blocks_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM simple_blocks_sources
    "${simple_blocks_SOURCE_DIR}/src/main.cpp"
)

file(GLOB simple_blocks_headers "${simple_blocks_SOURCE_DIR}/src"
    "${simple_blocks_SOURCE_DIR}/src/*.h"
)


set(simple_blocks_test_SOURCE_FILES
    src/simple_blocks_test_main.cpp
    src/simple_blocks_tests.cpp
    src/blockloader_test.cpp
    "${simple_blocks_sources}" )


set(simple_blocks_test_HEADER_FILES
    "${simple_blocks_headers}"
) 


include_directories(
    "${THIRDPARTY_DIR}/catch"
    "${THIRDPARTY_DIR}/tclap/include"
    "${CRUFT_INCLUDE_DIR}"
    "${simple_blocks_SOURCE_DIR}/src"
    )


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets)
find_package(Qt5Gui)


set(CRUFT_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
add_definitions(-DRESOURCE_FOLDER="${CRUFT_RESOURCE_DIR}")


add_executable(simple_blocks_test
    ${simple_blocks_test_HEADER_FILES}
    ${simple_blocks_test_SOURCE_FILES}
    )


target_link_libraries(simple_blocks_test
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${CRUFT_LIBRARIES}
        ${TBB_LIBRARIES}
        ${NVTOOLSEXT_LIB}
        Qt5::Widgets
        Qt5::Core)


install(TARGETS simple_blocks_test RUNTIME DESTINATION ${BIN_DIR})
