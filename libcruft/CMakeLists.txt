#
# <root>/libcruft/CMakeLists.txt
#

cmake_minimum_required(VERSION 2.8)

#### P r o j e c t   D e f i n i t i o n  ##################################
project(cruft CXX)

#### Graphics & Math Libraries ################################################
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

#### Platform Specifics ################################################
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
              -std=c++14 \
              -Wall \
              -Wno-missing-braces \
              -pthread \
              -fdiagnostics-color=auto"
    )
endif (UNIX)

# Resource directory is for test code only.
set(CRUFT_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)


add_subdirectory("include/bd")
add_subdirectory("src/bd")

# bd_SOURCES and bd_HEADERS are created in subdirectories and passed up to this cmake scope
add_library(cruft STATIC "${bd_SOURCES}" "${bd_HEADERS}")

if (UNIX)
    target_include_directories(cruft PUBLIC "${OPENGL_INCLUDE_DIR}")
endif()
target_include_directories(cruft PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${THIRDPARTY_DIR}/json/include")

target_link_libraries(cruft
    "${GLEW_LIBRARIES}"
    "${OPENGL_LIBRARIES}"
    "${GLFW_LIBRARIES}")

add_definitions(-DGLEW_STATIC)
if (WIN32)
    add_definitions(-DNOMINMAX)     #Disable the overrides of std::min/max in Windows.h
endif (WIN32)

add_compile_options(
    "$<$<CONFIG:Debug>:-DBD_DEBUG>"
    "$<$<CONFIG:Release>:>"
    )
set_target_properties(cruft PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG "cruftd")
set_target_properties(cruft PROPERTIES ARCHIVE_OUTPUT_NAME_RELEASE "cruft")


# Install the static library.
install(TARGETS cruft ARCHIVE DESTINATION "lib/bd/")

# Install the headers.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/bd/"
        DESTINATION "include/bd" FILES_MATCHING PATTERN "*.h")

# On unix, create a custom install target
# I find this useful for Jetbrain's CLion IDE.
if (UNIX)
add_custom_target(install_${PROJECT_NAME}
        make install
        DEPENDS ${PROJECT_NAME}
        COMMENT "Installing ${PROJECT_NAME}"
)
endif()

add_subdirectory("test")
