cmake_minimum_required(VERSION 2.8)

project(simplevr)

set(SOURCE_FILES 
    src/main.cpp
    src/block.cpp
    src/blockscollection.cpp
    src/cmdline.cpp
    src/geometry.cpp
    src/transferfunction.cpp
    src/view.cpp
    src/volrendloop.cpp
    src/volume.cpp
    )

set(HEADER_FILES 
    src/block.h
    src/blockscollection.h
    src/cmdline.h
    src/geometry.h
    src/transferfunction.h
    src/volrendloop.h
    src/volume.h
    ) 

#add_subdirectory(src)

include_directories("${SHARED_DIR}/include")

add_executable(simplevr ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(simplevr
                        ${OPENGL_LIBRARIES}
                        ${GLFW_LIBRARIES}
                        ${GLEW_LIBRARIES}
                        ${Boost_PROGRAM_OPTIONS_LIBRARIES}
                        ${Boost_LIBRARIES} 
                        shared
#                        ${FREETYPE_LIBRARIES}
                        )

install(TARGETS simplevr RUNTIME DESTINATION ${BIN_DIR})
