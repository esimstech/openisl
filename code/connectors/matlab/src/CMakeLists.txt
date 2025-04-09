set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_matlab_global.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_matlab.cpp"
)

if(FILES)
    target_sources("isl_matlab" PRIVATE ${FILES})
endif()