################################################################################
# Project definition
project(preprocessor)
include_directories("${CRUFT_INCLUDE_DIR}")
include_directories("${THIRDPART_DIR}/tclap/include")

################################################################################
# Sources
set(preprocessor_HEADERS
        src/blockingdatareader.h)

set(preprocessor_SOURCES
    src/main.cpp
    )


################################################################################
# Linker
add_executable(preprocessor "${preprocessor_HEADERS}" "${preprocessor_SOURCES}")
set_property( TARGET preprocessor APPEND_STRING PROPERTY COMPILE_FLAGS
        "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-documentation")

#target_link_libraries()

install(TARGETS preprocessor RUNTIME DESTINATION ${BIN_DIR})