#
# <root>/CMakeLists.txt
#

cmake_minimum_required(VERSION 2.8)

#### P r o j e c t   D e f i n i t i o n  ##################################
project(cruft CXX)

#### T h i r d   P a r t y  ################################################
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH}
)


#### Intel's Thread Building Blocks ################################################
if(NOT DEFINED INTELTBB_INCLUDE_DIR)
    set(INTELTBB_INCLUDE_DIR "")
endif()
set(INTELTBB_INCLUDE_DIR ${${INTELTBB_INCLUDE_DIR}} CACHE PATH 
	"Intel's thread building blocks includes path"
)

if(NOT DEFINED INTELTBB_LIBRARY_DIRS)
    set(INTELTBB_LIBRARY_DIRS "")
endif()
set(INTELTBB_LIBRARY_DIRS ${${INTELTBB_LIBRARY_DIRS}} CACHE PATH 
	"Intel's thread building blocks library."
)

#### Graphics & Math Libraries ################################################
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

#### Platform Specifics ################################################
# Windows requires a few extra definitions.

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
              -std=c++14 \
              -Wall \
              -Wno-missing-braces \
              -pthread \
              -fdiagnostics-color=auto"
    )

endif (UNIX)


#Link statically to GLEW

# Resource directory is for test code only.
set(CRUFT_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")

if (UNIX)
    include_directories("${OPENGL_INCLUDE_DIR}")
endif()

include_directories(
        "${GLEW_INCLUDE_DIR}"
        "${GLM_INCLUDE_DIR}"
        "${GLFW_INCLUDE_DIRS}"
        "${INTELTBB_INCLUDE_DIR}"
        )

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
link_directories("${INTELTBB_LIBRARY_DIRS}")

add_subdirectory("include/bd")
add_subdirectory("src/bd")


# bd_SOURCES and bd_HEADERS are created in subdirectories and passed up
# to this cmake scope--kinda dirty i think.
add_library(cruft STATIC "${bd_SOURCES}" "${bd_HEADERS}" )
target_link_libraries(cruft
    "${GLEW_LIBRARIES}"
    "${OPENGL_LIBRARIES}"
    "${GLFW_LIBRARIES}"
    debug tbb_debug
    optimized tbb
    )

add_definitions(-DGLEW_STATIC)

if (WIN32)
    add_definitions(-DNOMINMAX)     #Disable the overrides of std::min/max in Windows.h
endif (WIN32)

# Enables gl_check() macros.

add_compile_options(
    "$<$<CONFIG:Debug>:-DBD_DEBUG>"
    "$<$<CONFIG:Release>:>"
    )
set_target_properties(cruft PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG "cruftd")
set_target_properties(cruft PROPERTIES ARCHIVE_OUTPUT_NAME_RELEASE "cruft")



# Install the static library.
install(TARGETS cruft ARCHIVE
        DESTINATION "lib/bd/")

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
