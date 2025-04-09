set(PRIVATE_FILES
"${CMAKE_CURRENT_SOURCE_DIR}/isl_simulink_global.h"
"${CMAKE_CURRENT_SOURCE_DIR}/isl_simulink.h"
"${CMAKE_CURRENT_SOURCE_DIR}/logcodes.h"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("isl_simulink" PRIVATE ${FILES})
endif()
