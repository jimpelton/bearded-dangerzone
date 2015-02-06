
project(template)

set(HEADER_FILES 
    "${SHARED_DIR}/geometry/BBox.h" )

set(SOURCE_FILES src/main.cpp
    "${SHARED_DIR}/geometry/BBox.cpp" )

add_executable(template ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(template
                        ${OPENGL_LIBRARIES}
                        ${GLFW_LIBRARIES}
                        ${GLEW_LIBRARIES}
#                        ${Boost_PROGRAM_OPTIONS_LIBRARIES}
#                        ${Boost_LIBRARIES} 
#                        ${FREETYPE_LIBRARIES}
                        )

install(TARGETS template RUNTIME DESTINATION ${BIN_DIR})
