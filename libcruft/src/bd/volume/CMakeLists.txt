#
#  src/volume/CMakeLists.txt
#

set(volume_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/block.cpp"
  #  "${CMAKE_CURRENT_SOURCE_DIR}/blockcollection.cpp"
  #      "${CMAKE_CURRENT_SOURCE_DIR}/blockloader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opacitytransferfunction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/colortransferfunction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/volume.cpp"
    PARENT_SCOPE
    )
