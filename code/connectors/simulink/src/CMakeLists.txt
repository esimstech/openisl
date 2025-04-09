set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_simulink_global.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_simulink.cpp"
)

if(FILES)
    target_sources("isl_simulink" PRIVATE ${FILES})
endif()