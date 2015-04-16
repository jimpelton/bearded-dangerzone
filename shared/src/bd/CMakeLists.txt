#
# shared/src/bd
#

add_subdirectory(file)
add_subdirectory(graphics)
add_subdirectory(log)
#add_subdirectory(shader)
add_subdirectory(util)
add_subdirectory(scene)

#set(bd_HEADERS
#    "${file_HEADERS}"
#    "${graphics_HEADERS}"
#    "${log_HEADERS}"
#    "${util_HEADERS}"
#    PARENT_SCOPE
#    )

set(bd_SOURCES
    "${file_SOURCES}"
    "${graphics_SOURCES}"
    "${log_SOURCES}"
    "${scene_SOURCES}"
    "${util_SOURCES}"
    PARENT_SCOPE
    )



