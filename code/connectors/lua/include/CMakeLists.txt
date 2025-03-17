set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_lua_global.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_lua.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/logcodes.h"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("isl_lua" PRIVATE ${FILES})
endif()
