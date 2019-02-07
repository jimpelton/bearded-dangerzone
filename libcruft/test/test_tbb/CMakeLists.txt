#
# <root>/test/test_tbb/CMakeLists.txt
#


add_executable(test_tbb test_tbb_main.cpp test_ParallelVoxelClassifer.cpp)
target_link_libraries(test_tbb cruft)

