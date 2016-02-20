################################################################################
# Project definition
project(preprocessor)

include_directories("${CRUFT_INCLUDE_DIR}")
include_directories("${THIRDPARTY_DIR}/tclap/include")

################################################################################
# Sources
set(preprocessor_HEADERS
        src/blockcollection2.h
        src/cmdline.h
    )

set(preprocessor_SOURCES
        src/main.cpp
        src/blockcollection2.cpp
        src/cmdline.cpp
    )


################################################################################
# Linker
add_executable(preprocessor "${preprocessor_HEADERS}" "${preprocessor_SOURCES}")


target_link_libraries(preprocessor
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${CRUFT_LIBRARIES}
        )

################################################################################
# Compiler options for Clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set_property( TARGET preprocessor APPEND_STRING PROPERTY COMPILE_FLAGS
            "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-documentation")
endif()

#target_link_libraries()

install(TARGETS preprocessor RUNTIME DESTINATION ${BIN_DIR})