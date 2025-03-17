set(PUBLIC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/process3.xml"
)

if(PUBLIC_FILES)
    install(FILES ${PUBLIC_FILES} DESTINATION "examples/tokens/process3")
endif()