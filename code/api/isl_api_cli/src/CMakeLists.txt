set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_cli.cpp"
)

if(FILES)
    target_sources("isl_cli" PRIVATE ${FILES})
endif()