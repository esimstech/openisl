set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/fmi2Functions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/islfmuslave.cpp"
)

if(FILES)
    target_sources("isl_slavefmi" PRIVATE ${FILES})
endif()