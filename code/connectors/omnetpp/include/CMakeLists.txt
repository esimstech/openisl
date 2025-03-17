set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/logcodes.h"
)

set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_omnetpp_global.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_omnetpp.h"
)

set(FILES ${PRIVATE_FILES} ${PUBLIC_FILES})

if(FILES)
    target_sources("isl_omnetpp" PRIVATE ${FILES})
endif()

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "inc")
endif()