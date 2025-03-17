set(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

if(FILES)
    target_sources("process3" PRIVATE ${FILES})
endif()