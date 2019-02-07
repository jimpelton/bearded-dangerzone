

#add_subdirectory(context)
add_subdirectory(datastructure)
add_subdirectory(filter)
add_subdirectory(geo)
add_subdirectory(graphics)
add_subdirectory(io)
add_subdirectory(log)
add_subdirectory(scene)
# add_subdirectory(tbb)
add_subdirectory(util)
add_subdirectory(volume)




set(bd_HEADERS
    "${datastructure_HEADERS}"
    "${io_HEADERS}"
    "${filter_HEADERS}"
    "${geo_HEADERS}"
    "${graphics_HEADERS}"
    "${log_HEADERS}"
    "${scene_HEADERS}"
#   "${tbb_HEADERS}"
    "${util_HEADERS}"
    "${volume_HEADERS}"
    PARENT_SCOPE
    )