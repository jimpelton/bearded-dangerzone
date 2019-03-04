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
    add_compile_options(
        "$<$<CONFIG:Debug>:-DUSE_NV_TOOLS>"
        "$<$<CONFIG:Release>:-DUSE_NV_TOOLS>"
    )
    
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
    add_definitions(-DSUBVOL_PROF)
    if(NOT DEFINED NVPMAPI_INCLUDE_DIR)
        set(NVPMAPI_INCLUDE_DIR "")
    endif()
    set(NVPMAPI_INCLUDE_DIR ${${NVPMAPI_INCLUDE_DIR}} CACHE PATH "NvPmApi includes path")

# if(NOT DEFINED NVPMAPI_LIB)
#     set(NVPMAPI_LIB "")
# endif()
# set(NVPMAPI_LIB ${${NVPMAPI_LIB}} CACHE FILEPATH "NvPmApi library file")
# add_definitions(-DPATH_TO_NVPMAPI_CORE="${NVPMAPI_LIB}")
endif(USE_NVPMAPI)


################################################################################
# Project definition
project(simple_blocks)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Qt5Widgets)
find_package(Qt5Gui)

################################################################################
# Sources
set(simple_blocks_HEADERS
        src/axis_enum.h
        src/io/blockcollection.h
        src/io/blockloader.h
        src/classificationtype.h
        src/cmdline.h
        src/colormap.h
        src/controls.h
        src/constants.h
        src/controlpanel.h
        src/renderer/create_vao.h
        src/loop.h
        # src/nvpm.h
        src/nvtools.h
        src/renderhelp.h
        src/semathing.h
        src/sliceset.h
        src/timing.h
        
        src/messages/message.h
        src/messages/messagebroker.h
        src/messages/recipient.h
        src/messages/rovchangingmessage.h
        src/renderer/slicingblockrenderer.h
        src/renderer/blockraycaster.h
        src/renderer/blockrenderer.h)

set(simple_blocks_SOURCES
        src/main.cpp
        src/io/blockcollection.cpp
        src/io/blockloader.cpp
        src/cmdline.cpp
        src/colormap.cpp
        src/constants.cpp
        src/controls.cpp
        src/controlpanel.cpp
        src/renderer/create_vao.cpp
        src/loop.cpp
        # src/nvpm.cpp
        src/sliceset.cpp
        src/timing.cpp
        src/renderhelp.cpp

        src/messages/messagebroker.cpp
        src/renderer/slicingblockrenderer.cpp
        src/renderer/blockraycaster.cpp
        src/renderer/blockrenderer.cpp)


add_executable(simple_blocks "${simple_blocks_HEADERS}" "${simple_blocks_SOURCES}")

if (USE_NVTOOLSEXT)
    target_include_directories(simple_blocks PUBLIC "${NVTOOLSEXT_INCLUDE_DIR}")
endif(USE_NVTOOLSEXT)

if (USE_NVPMAPI)
    target_include_directories(simple_blocks PUBLIC "${NVPMAPI_INCLUDE_DIR}")
endif(USE_NVPMAPI)

target_include_directories(simple_blocks PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        "${CRUFT_INCLUDE_DIR}"
        "${THIRDPARTY_DIR}/tclap/include"
        "${GLEW_INCLUDE_DIR}"
        "${GLM_INCLUDE_DIR}"
        "${GLFW_INCLUDE_DIRS}")

#find_library(CRUFT_DEBUG_LIB cruftd HINTS "${CRUFT_LIBRARY_DIR}")
#find_library(CRUFT_RELEASE_LIB cruft HINTS "${CRUFT_LIBRARY_DIR}")

target_link_libraries(simple_blocks PUBLIC
        cruft
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
#        debug ${CRUFT_DEBUG_LIB}
#        optimized ${CRUFT_RELEASE_LIB}
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
