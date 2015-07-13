#
# <root>/test/simplevr_test
#

cmake_minimum_required(VERSION 2.8)

project(simplevr_test)

file(GLOB simplevr_sources "${simplevr_SOURCE_DIR}/src"
    "${simplevr_SOURCE_DIR}/src/*.cpp"
    )

list(REMOVE_ITEM simplevr_sources
    "${simplevr_SOURCE_DIR}/src/main.cpp"
    )

file(GLOB simplevr_headers "${simplevr_SOURCE_DIR}/src"
    "${simplevr_SOURCE_DIR}/src/*.h")

message(STATUS "simplevr_sources: ${simplevr_sources}")
message(STATUS "simplevr_headers: ${simplevr_headers}")



set(simplevr_test_SOURCE_FILES
    "${simplevr_sources}"
    )

set(simplevr_test_HEADER_FILES
    "${simplevr_headers}"
    ) 


include_directories(
    "${THIRDPARTY_DIR}/catch"
    "${cruft_SOURCE_DIR}/include"
    "${simplevr_SOURCE_DIR}/src"
    )


add_executable(simplevr_test
    src/test_main.cpp
    ${simplevr_test_HEADER_FILES}
    ${simplevr_test_SOURCE_FILES}
    )

add_executable(simplevr_test2
    src/test_main2.cpp
    ${simplevr_test_HEADER_FILES}
    ${simplevr_test_SOURCE_FILES}
    )



target_link_libraries(simplevr_test
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    ${Boost_LIBRARIES}
    shared
    #${FREETYPE_LIBRARIES}
    )

target_link_libraries(simplevr_test2
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    ${Boost_LIBRARIES}
    shared
    #${FREETYPE_LIBRARIES}
    )

install(TARGETS simplevr_test RUNTIME DESTINATION ${BIN_DIR})
