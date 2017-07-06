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

include_directories("${CRUFT_INCLUDE_DIR}"
        "${TBB_INCLUDE_DIR}"
        "${THIRDPARTY_DIR}/tclap/include")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)
find_package(Qt5Gui)

################################################################################
# Sources
set(simple_blocks_HEADERS
        src/axis_enum.h
        src/blockcollection.h
        src/blockloader.h
        src/blockrenderer.h
        src/classificationtype.h
        src/cmdline.h
        src/colormap.h
        src/controls.h
        src/constants.h
        src/controlpanel.h
        src/create_vao.h
        src/loop.h
        src/nvpm.h
        src/renderhelp.h
        src/sliceset.h
        src/semathing.h
        src/timing.h
        )

set(simple_blocks_SOURCES
        src/main.cpp
        src/blockcollection.cpp
        src/blockloader.cpp
        src/blockrenderer.cpp
        src/cmdline.cpp
        src/colormap.cpp
        src/constants.cpp
        src/controls.cpp
        src/controlpanel.cpp
        src/create_vao.cpp
        src/loop.cpp
        src/sliceset.cpp
        src/timing.cpp
        src/renderhelp.cpp )


################################################################################
# Linker
add_executable(simple_blocks "${simple_blocks_HEADERS}" "${simple_blocks_SOURCES}")

target_link_libraries(simple_blocks
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${CRUFT_LIBRARIES}
        ${TBB_LIBRARIES}
        ${NVTOOLSEXT_LIB}
        Qt5::Widgets
        Qt5::Core)



################################################################################
# Copy shaders folder to the build directory.
add_custom_command(TARGET simple_blocks POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:simple_blocks>/shaders)

install(TARGETS simple_blocks RUNTIME DESTINATION "bin/")

if (UNIX)
    add_custom_target(install_${PROJECT_NAME}
            make install
            DEPENDS ${PROJECT_NAME}
            COMMENT "Installing ${PROJECT_NAME}"
            )
endif()
