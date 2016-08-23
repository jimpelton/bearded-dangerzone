
cmake_minimum_required(VERSION 2.8)

include_directories("${CRUFT_INCLUDE_DIR}")
include_directories("${THIRDPARTY_DIR}/tclap/include")

################################################################################
# Project definition
project(rawhist)

################################################################################
# Sources
set(rawhist_HEADERS)

set(rawhist_SOURCES
        src/main.cpp)


################################################################################
# Target
add_executable(rawhist "${rawhist_HEADERS}" "${rawhist_SOURCES}")


################################################################################
# Linker


################################################################################
# Install
install(TARGETS rawhist RUNTIME DESTINATION ${BIN_DIR})
