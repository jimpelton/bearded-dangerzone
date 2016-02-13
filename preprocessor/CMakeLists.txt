################################################################################
# Project definition
project(preprocessor)

include_directories("${CRUFT_INCLUDE_DIR}")
include_directories("${THIRDPART_DIR}/tclap/include")

################################################################################
# Sources
set(preprocessor_HEADERS
)

set(preprocessor_SOURCES
    src/main.cpp
    )


################################################################################
# Linker
add_executable(preprocessor "${preprocessor_HEADERS}" "${preprocessor_SOURCES}")

#target_link_libraries()

install(TARGETS preprocessor RUNTIME DESTINATION ${BIN_DIR})