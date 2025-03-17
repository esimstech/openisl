set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_modelica_global.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_modelica.cpp"
)

if(FILES)
    target_sources("isl_modelica" PRIVATE ${FILES})
endif()