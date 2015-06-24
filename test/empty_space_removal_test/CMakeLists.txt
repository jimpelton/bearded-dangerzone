#
# <root>/test/empty_space_removal_test
#

cmake_minimum_required(VERSION 2.8)

project(empty_space_removal_test)

file(GLOB empty_space_removal_sources "${empty_space_removal_SOURCE_DIR}/src"
    "${empty_space_removal_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM empty_space_removal_sources
    "${empty_space_removal_SOURCE_DIR}/src/main.cpp"
)

file(GLOB empty_space_removal_headers "${empty_space_removal_SOURCE_DIR}/src"
    "${empty_space_removal_SOURCE_DIR}/src/*.h"
)

set(empty_space_removal_test_SOURCE_FILES
    "src/main.cpp"
    "${empty_space_removal_sources}"
)

set(empty_space_removal_test_HEADER_FILES
    "${empty_space_removal_headers}"
) 


include_directories(
    "${THIRDPARTY_DIR}/catch"
    "${THIRDPARTY_DIR}/tclap/include"
    "${cruft_SOURCE_DIR}/include"
    "${empty_space_removal_SOURCE_DIR}/src"
    )


add_executable(empty_space_removal_test
    ${empty_space_removal_test_HEADER_FILES}
    ${empty_space_removal_test_SOURCE_FILES}
    )


target_link_libraries(empty_space_removal_test
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    ${Boost_LIBRARIES}
    crufterly
    )


install(TARGETS empty_space_removal_test RUNTIME DESTINATION ${BIN_DIR})
