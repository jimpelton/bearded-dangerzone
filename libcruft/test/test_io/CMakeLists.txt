#
# <root>/test/test_volume/CMakeLists.txt
#


#project(test_util)
add_executable(test_io test_io_main.cpp
        test_indexfile.cpp
        )


target_link_libraries(test_io cruft)
