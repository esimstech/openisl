set(PRIVATE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/app.ico"
    "${CMAKE_CURRENT_SOURCE_DIR}/app.rc"
)

set(FILES ${PRIVATE_FILES})

if(FILES)
    target_sources("isl_cli" PRIVATE ${FILES})
endif()
