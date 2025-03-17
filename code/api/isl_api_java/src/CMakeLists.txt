set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/isl_java.cpp"
)

if(FILES)
    target_sources("isl_java" PRIVATE ${FILES})
endif()