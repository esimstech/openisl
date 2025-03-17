set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/logcodes.h"
)

set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_modelica_global.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_modelica.h"
)

set(FILES ${PRIVATE_FILES} ${PUBLIC_FILES})

if(FILES)
    target_sources("isl_modelica" PRIVATE ${FILES})
endif()

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "connect/openmodelica/OpenISL/Resources/Include")
endif()