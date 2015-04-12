#
#  shared/src/util
#

#set(util_HEADERS
#    "${CMAKE_CURRENT_SOURCE_DIR}/boundingbox.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/context.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/contextcontroller.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/glfwcontext.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/shader.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/texture.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/transform.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/transformable.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/util.h"
#    PARENT_SCOPE
#    )


set(util_SOURCES
    "${SHARED_SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/boundingbox.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/glfwcontext.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/transform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/transformable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/util.cpp"
    PARENT_SCOPE
    )

