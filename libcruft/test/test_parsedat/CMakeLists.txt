#
# <root>/test/test_parsedat/CMakeLists.txt
#


#project(test_parsedat)
add_executable(test_parsedat test_parsedat_main.cpp)
target_link_libraries(test_parsedat cruft ${GLFW_LIBRARIES})


