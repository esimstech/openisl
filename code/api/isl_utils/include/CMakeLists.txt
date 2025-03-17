set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_ini.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_log.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_misc.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_sem.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_shm.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_thread.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_xml.h"
)

if(FILES)
    target_sources("isl_utils" PRIVATE ${FILES})
endif()