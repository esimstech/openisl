set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/process1.xml"
)

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "examples/tokens/process1")
endif()