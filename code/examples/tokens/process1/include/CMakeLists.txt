set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/swversion.h"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("process1" PRIVATE ${FILES})
endif()
