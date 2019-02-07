#
#  shared/src/util
#

set(util_SOURCES
    "${SHARED_SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/bdobj.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/color.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/util.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/gl_strings.cpp"
    PARENT_SCOPE
    )

