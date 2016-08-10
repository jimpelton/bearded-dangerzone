#
# <root>/simple_blocks/
#

cmake_minimum_required(VERSION 2.8)

################################################################################
# Nvidia Visual Tools Extensions
if(NOT DEFINED USE_NVTOOLSEXT)
    set(USE_NVTOOLSEXT FALSE)
endif()
set(USE_NVTOOLSEXT ${${SET_NVPROFILE}} CACHE BOOL "True if profiling should be enabled, false for not profiling")

if (USE_NVTOOLSEXT)
    add_definitions(-DBDPROF)

    if(NOT DEFINED NVTOOLSEXT_INCLUDE_DIR)
        set(NVTOOLSEXT_INCLUDE_DIR "")
    endif()
    set(NVTOOLSEXT_INCLUDE_DIR ${${NVTOOLSEXT_INCLUDE_DIR}} CACHE PATH "NvToolsExt includes path")

    if(NOT DEFINED NVTOOLSEXT_LIB)
        set(NVTOOLSEXT_LIB "")
    endif()
    set(NVTOOLSEXT_LIB ${${NVTOOLSEXT_LIB}} CACHE FILEPATH "NvToolsExt library")

endif(USE_NVTOOLSEXT) 


################################################################################
# Nvidia Perfmon Api (access cpu counter information)
if(NOT DEFINED USE_NVPMAPI)
    set(USE_NVPMAPI FALSE)
endif()
set(USE_NVPMAPI ${${USE_NVPROFILE}} CACHE BOOL "True if nvidia's perfmon should be used.")

if (USE_NVPMAPI)
    add_definitions(-DBDPROF)
    if(NOT DEFINED NVPMAPI_INCLUDE_DIR)
        set(NVPMAPI_INCLUDE_DIR "")
    endif()
    set(NVPMAPI_INCLUDE_DIR ${${NVPMAPI_INCLUDE_DIR}} CACHE PATH "NvPmApi includes path")

    # if(NOT DEFINED NVPMAPI_LIB)
    #     set(NVPMAPI_LIB "")
    # endif()
    # set(NVPMAPI_LIB ${${NVTOOLSEXT_LIB}} CACHE FILEPATH "NvPmApi library file")
endif(USE_NVPMAPI)




################################################################################
# Project definition
project(simple_blocks)

if (USE_NVTOOLSEXT)
    include_directories("${NVTOOLSEXT_INCLUDE_DIR}")
endif(USE_NVTOOLSEXT)

if (USE_NVPMAPI)
    include_directories("${NVPMAPI_INCLUDE_DIR}")
endif(USE_NVPMAPI)

include_directories("${CRUFT_INCLUDE_DIR}")
include_directories("${THIRDPARTY_DIR}/tclap/include")


################################################################################
# Sources
set(simple_blocks_HEADERS
        src/axis_enum.h
        src/blockrenderer.h
        src/cmdline.h
        src/create_vao.h
        src/glfw_callbacks.h
        src/nvpm.h
        src/sliceset.h
        src/timing.h
        src/constants.h
        src/colormap.h
        src/renderer.h
        src/renderhelp.h )

set(simple_blocks_SOURCES
        src/main.cpp
        src/cmdline.cpp
        src/create_vao.cpp
        src/glfw_callbacks.cpp
        src/load_transfer.cpp
        src/sliceset.cpp
        src/timing.cpp
        src/blockrenderer.cpp
        src/colormap.cpp
        src/constants.cpp
        src/renderer.cpp
        src/renderhelp.cpp)


################################################################################
# Linker
add_executable(simple_blocks "${simple_blocks_HEADERS}" "${simple_blocks_SOURCES}")

target_link_libraries(simple_blocks
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        debug ${CRUFT_DEBUG_LIBRARIES}
        optimized ${CRUFT_RELEASE_LIBRARIES}
        ${NVTOOLSEXT_LIB}
        /usr/lib/libtbb.so
#    /usr/local/lib/libtbb.dylib
)



################################################################################
# Copy shaders folder to the build directory.
add_custom_command(TARGET simple_blocks POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:simple_blocks>/shaders)

install(TARGETS simple_blocks RUNTIME DESTINATION ${BIN_DIR})
