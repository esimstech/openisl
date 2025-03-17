set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/appsettings.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/fmusim_const.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/fmusim.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/killerthread.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/model.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/modelvar.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/runthread.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/slave_v2_0.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/slave.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/swversion.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils.h"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("islmasterfmi" PRIVATE ${FILES})
endif()
