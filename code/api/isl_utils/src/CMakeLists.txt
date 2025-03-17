set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_ini.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_misc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_sem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_shm.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_xml.cpp"
)

if(FILES)
    target_sources("isl_utils" PRIVATE ${FILES})
endif()