#
# <root>/test/shared_test
#

cmake_minimum_required(VERSION 2.8)

project(shared_test)

#set(shared_test_SOURCE_FILES
#    "${shared_SOURCE_DIR}/src"
#    )

#set(shared_test_HEADER_FILES
#    "${shared_SOURCE_DIR}/include"
#    )


include_directories(
    "${THIRDPARTY_DIR}/catch"
    "${shared_SOURCE_DIR}/include"
    )


add_executable(transform_test
    src/transform_test.cpp
    )


#add_executable(transformable_test
#    src/transformable_test.cpp
#    )



target_link_libraries(transform_test
    ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} shared
    )


#target_link_libraries(transformable_test
#    ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} shared
#    )

#install(TARGETS transform_test RUNTIME DESTINATION ${BIN_DIR})
