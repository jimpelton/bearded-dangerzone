#
# <root>/CMakeLists.txt
#

cmake_minimum_required(VERSION 2.8)

#### P r o j e c t   D e f i n i t i o n  ##################################
project(resample LANGUAGES CXX)

#### Definitions ##########################################################
# Windows requires a few extra definitions.
#if (WIN32)
#    add_definitions(-DNOMINMAX)     #Disable the overrides of std::min/max in Windows.h
#endif (WIN32)

#if (UNIX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
#    -std=c++17 -Wall \
#    -Wno-missing-braces \
#   -fdiagnostics-color=auto \
#    -pthread" )
#endif (UNIX)


################################################################################
# Sources
set(concat_HEADERS
        src/cmdline.h
        src/grid.h)

set(concat_SOURCES
        src/cmdline.cpp
        src/main.cpp)


################################################################################
# Target
add_executable(resample "${concat_HEADERS}" "${concat_SOURCES}")

target_link_libraries(resample PUBLIC cruft)

target_include_directories(resample PUBLIC
        "${THIRDPARTY_DIR}/tclap/include"
        "${CRUFT_INCLUDE_DIR}"
)


install(TARGETS resample RUNTIME DESTINATION "bin/")

add_custom_target(install_${PROJECT_NAME}
        make install
        DEPENDS ${PROJECT_NAME}
        COMMENT "Installing ${PROJECT_NAME}")
