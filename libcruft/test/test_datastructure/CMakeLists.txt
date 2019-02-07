#
# <root>/test/test_datastructure/CMakeLists.txt
#


#project(test_util)
add_executable(test_datastructure test_datastructure_main.cpp test_octree.cpp)
target_link_libraries(test_datastructure cruft)
