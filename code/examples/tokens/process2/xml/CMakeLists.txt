set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/process2.xml"
)

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "examples/tokens/process2")
endif()