
project(simplevr)

set(HEADER_FILES 
    "${SHARED_DIR}/geometry/BBox.h" 
    "${SHARED_DIR}/log/gl_log.h")

set(SOURCE_FILES src/main.cpp
    "${SHARED_DIR}/geometry/BBox.cpp"
    "${SHARED_DIR}/log/gl_log.cpp" )

add_subdirectory(src)

add_executable(simplevr ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(simplevr
                        ${OPENGL_LIBRARIES}
                        ${GLFW_LIBRARIES}
                        ${GLEW_LIBRARIES}
#                        ${Boost_PROGRAM_OPTIONS_LIBRARIES}
#                        ${Boost_LIBRARIES} 
#                        ${FREETYPE_LIBRARIES}
                        )

install(TARGETS simplevr RUNTIME DESTINATION ${BIN_DIR})
