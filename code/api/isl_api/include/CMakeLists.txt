set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_exitthread.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_shm_connect.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_shm_data.h"
)

set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_api.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_api_c.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_api_global.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_connect.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_const.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_data.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_errorcodes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_instances.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_settings.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_simulations.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_utils.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_variable.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/swversion.h"
)

set(FILES ${PRIVATE_FILES} ${PUBLIC_FILES})

if(FILES)
    target_sources("isl_api" PRIVATE ${FILES})
endif()

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "inc")
endif()