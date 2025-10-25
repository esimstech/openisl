set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/islfmuslave.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/logcodes.h"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("isl_slavefmi" PRIVATE ${FILES})
endif()
