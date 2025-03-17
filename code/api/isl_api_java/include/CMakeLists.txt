set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_java.h"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("isl_java" PRIVATE ${FILES})
endif()
