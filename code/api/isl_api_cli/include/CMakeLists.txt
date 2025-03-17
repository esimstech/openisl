set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_cli.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/resource.h"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("isl_cli" PRIVATE ${FILES})
endif()
