set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_api_c.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_api_global.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_connect.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_data.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_exitthread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_instances.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_settings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_shm_connect.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_shm_data.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_simulations.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_utils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_variable.cpp"
)

if(FILES)
    target_sources("isl_api" PRIVATE ${FILES})
endif()