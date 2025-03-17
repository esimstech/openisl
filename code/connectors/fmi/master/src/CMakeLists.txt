set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/appsettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fmusim.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/killerthread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/model.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/modelvar.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runthread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/slave_v2_0.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/slave.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp"
)

if(FILES)
    target_sources("islmasterfmi" PRIVATE ${FILES})
endif()