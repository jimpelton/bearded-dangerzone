#
#  include/io/CMakeLists.txt
#

set(io_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/buffer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/bufferedreader.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/bufferpool.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/datatypes.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/datfile.h"
       # "${CMAKE_CURRENT_SOURCE_DIR}/fileblockcollection.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/fileblock.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/indexfile.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/indexfileheader.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/readerworker.h"
        PARENT_SCOPE
        )
