
project(inst)

set(HEADER_FILES 
    "${SHARED_DIR}/geometry/BBox.h"
    "${SHARED_DIR}/geometry/mesh.h"
    "${SHARED_DIR}/file/tiny_obj_loader/tiny_obj_loader.h")

set(SOURCE_FILES src/instmain.cpp
    "${SHARED_DIR}/geometry/BBox.cpp"
    "${SHARED_DIR}/geometry/mesh.cpp"
    "${SHARED_DIR}/file/tiny_obj_loader/tiny_obj_loader.cc")

add_executable(inst ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(inst
                        ${OPENGL_LIBRARIES}
                        ${GLFW_LIBRARIES}
                        ${GLEW_LIBRARIES}
#                        ${Boost_PROGRAM_OPTIONS_LIBRARIES}
#                        ${Boost_LIBRARIES} 
#                        ${FREETYPE_LIBRARIES}
                        )

install(TARGETS inst RUNTIME DESTINATION ${BIN_DIR})
