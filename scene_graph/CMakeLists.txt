

cmake_minimum_required(VERSION 2.8)

project(scene_graph)

set(scene_graph_SOURCE_FILES
    main.cpp
    simplerenderloop.cpp
    )

set(simplevr_HEADER_FILES
    simplerenderloop.h
    )


#add_definitions(-DVR_SHADER_PATH="${simplevr_SOURCE_DIR}/shader")

include_directories("${SHARED_DIR}/include")

add_executable(scene_graph ${scene_graph_HEADER_FILES} ${scene_graph_SOURCE_FILES})

target_link_libraries(
    scene_graph
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    ${Boost_LIBRARIES}
#   ${FREETYPE_LIBRARIES}
    shared
    )


install(TARGETS scene_graph RUNTIME DESTINATION ${BIN_DIR})